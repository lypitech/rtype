# RFC: UDP-Based Real-Time Multiplayer Game Networking Protocol

## Table of Contents
1. Introduction
2. Terminology
3. Design Goals
4. Overview of Architecture
5. Packet Structure
6. Sequence Numbering and Acknowledgment System
7. Packet Handling Rules
8. Reliable Message Layer
9. Client-Side Interpolation and Prediction
10. Server-Side State Synchronization
11. Security Considerations
12. Example Message Flow
13. Conclusion

## 1. Introduction
This document describes a UDP-based networking protocol intended for real-time client-server multiplayer games.  
The protocol provides:
- Low-latency
- Connectionless communication while handling packet loss
- Duplication
- Out-of-order delivery through lightweight reliability and ordering mechanisms layered on top of UDP.

The goal of the protocol is to support real-time multiplayer games requiring continuous world-state synchronization.

## 2. Terminology
**Client** - A game application connecting to a server.

**Server** - The authoritative game host that maintains the global state.

**Snapshot** - A periodic description of the game world or relevant entities.

**Reliable Message** - A message requiring guaranteed delivery.

**Sequence Number** - A monotonically increasing identifier applied to each outgoing packet.

**Acknowledgment (ACK)** - A numeric value identifying the highest received sequence number, plus a bitmask indicating recently received packets.

## 3. Design Goals
The protocol is designed around the following goals:

1. **Low latency** suitable for real-time interactions.
2. **Graceful handling of packet loss** without stalling communication.
3. **Server authority**, with the server acting as the source of truth.
4. **Support for unreliable and reliable messages** within the same UDP channel.
5. **Smooth client-side experience** using interpolation and prediction.
6. **Scalable implementation** supporting many clients.

## 4. Overview of Architecture
The protocol operates over UDP.  
The server listens on a UDP socket and receives messages from clients, while periodically broadcasting world-state snapshots.  
Clients send periodic input updates.

A lightweight reliability layer is implemented on top of UDP using sequence numbers and acknowledgment bitmasks.  
This layer supports both unreliable real-time updates and optional reliable messages.

## 5. Packet Structure
All packets begin with a standardized header that supports:
- sequencing
- Acknowledgment
- Timestamp echoing
- Message classification.
- Order grouping for reliable message
- Reliable messages ID

This section defines the updated header format incorporating timestamp echo support.

### 5.1 Packet Header
```
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Type           |   order   |  Reserved |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Sequence                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Ack                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Ack Bits                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  Timestamp Echo                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  Ack Delay (ms)                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    reliable_id                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Reserved2                   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

```

Example in c:
```c
struct PacketHeaderReliable {
    uint16_t type;
    uint8_t order_group;   // for reliable
    uint8_t reserved;      // unused

    uint32_t seq;
    uint32_t ack;
    uint32_t ack_bits;

    uint32_t timestamp;
    uint32_t ack_delay;

    uint32_t reliable_id;  // for reliable
    uint32_t reserved2;    // unused
};
```

### 5.2 Header Fields
- **Type (16 bits)** - Identifies the packet type (snapshot, input, reliable event, etc.).
- **Order group (8 bits)** - used for reliable ordered streaming (ex: game chat)
- **Reserved (8 bits)** - Unused (padding)
- **Sequence (32 bits)** - Monotonically increasing packet identifier.
- **Ack (32 bits)** - Highest sequence number received from the peer.
- **Ack Bits (32 bits)** - Bitmask where bit *n* indicates receipt of (Ack - n - 1).
- **Timestamp Echo (32 bits)** - The sender echoes the timestamp (in ms) attached to the last acknowledged packet received from the peer.
- **Ack Delay (32 bits)** - Time in ms between the moment the acknowledged packet was received and the moment this acknowledgment was generated.
- **Reliable Message ID (32 bits)** - 
- **Reserved2  (32 bits)** - Unused (padding)

Total is a 32 bytes header.  
Payload content depends on the packet type.

## 6. Sequence Numbering, Acknowledgment System, and Timestamp Echo
This section defines the mechanisms used to detect:
- Packet loss
- Packet duplication
- Out-of-order packets
- round-trip time (RTT) computation

### 6.1 Sequence Numbers
Each endpoint maintains a 32-bit unsigned sequence counter that increments for every outgoing packet.  
Sequence numbers MUST be compared using serial number arithmetic (RFC 1982) to correctly handle wrap-around.

### 6.2 Acknowledgment Field (Ack)
The **Ack** field in each outgoing packet contains the highest sequence number received *in order* from the peer.  
This value communicates to the peer that all packets up to **Ack** have been successfully received.

### 6.3 Circular Acknowledgment Bitmask (Ack Bits)
The **Ack Bits** field communicates the reception status of the *previous 32 packets* before **Ack**.
- Bit 0 corresponds to `(Ack - 1)`
- Bit 1 corresponds to `(Ack - 2)`
- ...
- Bit 31 corresponds to `(Ack - 32)`

If a bit is **1**, the corresponding packet was received; if **0**, it was not received.

This structure forms a **circular acknowledgment window** updated for every incoming packet.  
The sender can reconstruct the complete reception state for the last 33 packets (Ack + AckBits).  
This allows precise detection of packet loss and acknowledgments even when packets themselves arrive out of order.

## 6.4 Timestamp Echoing for RTT Calculation
When an endpoint receives a packet, it immediately records the packet's timestamp field as `recv_timestamp`. When sending its next packet, it places the peer's original timestamp into the `Timestamp Echo` field.

This mechanism ensures that RTT reflects only network latency, because:
- The sender measures the interval between now and the timestamp echoed back.
- The receiver subtracts `Ack Delay` to remove server/client processing delays.

This avoids inflated RTT values caused by server tick rates or buffering.

## 6.5 Ack Delay
`Ack Delay` represents the elapsed time between:
1. When the acknowledged packet was received.
2. When the current packet (which contains the acknowledgment) was generated.

This allows the sender to subtract processing/tick delays from the RTT computation.

## 6.6 Detecting Packet Loss
Packet loss is inferred when:
- A sequence gap occurs (`seq > last_seq + 1`).
- AckBits indicate missing packets.

Lost unreliable packets are ignored. Lost reliable packets are scheduled for retransmission (Section 8).

## 6.7 Detecting Duplicate Packets
A packet is a duplicate if both:
1. Its sequence number is lesser or eq than `Ack`
2. Is represented by a 1-bit in `AckBits`

Duplicates MUST be discarded.

## 6.8 Handling Out-of-Order Packets
Out-of-order packets are expected in UDP transport.
- If a packet is newer than `Ack`, it is processed and the acknowledgment window is updated.
- If older, it may be buffered if useful (e.g., snapshot interpolation) or discarded.

## 7. Packet Handling Rules
This section describes standardized behavior for managing loss, duplication, and reordering.

### 7.1 Packet Loss
Packet loss occurs when a packet is never received or arrives outside the acknowledgment window.  
Loss is detected when a gap is visible in either the incoming sequence stream, or the peer's Ack+AckBits feedback.

Unreliable messages do not require retransmission.  
Reliable messages MUST be scheduled for retransmission (Section 8).

### 7.2 Duplicate Packets
A duplicate packet is one whose sequence number has already been acknowledged or processed.  
The receiver MUST discard duplicates immediately.

### 7.3 Out-of-Order Packets
UDP does not guarantee ordering.  
When an out-of-order packet arrives:
- Valid but older packets MAY be used for interpolation or snapshot buffering.
- Packets older than the last fully applied game state update SHOULD be discarded.

Endpoints MUST NOT treat out-of-order arrival as an error: it is expected behavior.

## 7.4 Summary Table
| Condition    | Detection Method                           | Required Action                       |
| ------------ | ------------------------------------------ | ------------------------------------- |
| Packet loss  | Missing bits in AckBits or sequence gaps   | Retransmit only reliable messages     |
| Duplicate    | Sequence already processed or acknowledged | Drop the packet                       |
| Out-of-order | Sequence < highest received                | Use only if useful; otherwise discard |

## 8. Reliable Message Layer
The Reliable Message Layer adds optional message-level reliability on top of the base protocol. Instead of using nested headers, the **base packet header is extended** with reliability-related fields when needed.

This keeps packets compact and simplifies parsing.

### 8.1 Extended Header for Reliable Messages

#### 8.1.1 If the packet contains reliable messages, the header becomes:
```c
struct PacketHeaderReliable {
    uint16_t type;
    uint8_t order_group;   // ordering group 0..255
    uint8_t reserved;      // unused

    uint32_t seq;
    uint32_t ack;
    uint32_t ack_bits;

    uint32_t timestamp;
    uint32_t ack_delay;

    uint32_t reliable_id;  // ReliableMessageID for this packet
    uint32_t reserved2;    // unused
};
```

#### 8.1.2 Key points:
- **Order group replaces the old "reserved" field.**
- **reliable_id and reliable_length are included only if RELIABLE_MESSAGE_PRESENT is set.**
- For packets without reliable messages, the original compact header is used.

### 8.2 How a Packet Is Explicitly Marked as Reliable
Inside `type` we dedicate one bit:
```
RELIABLE_MESSAGE_PRESENT = 0x8000  // highest bit
```

If this bit is set:
- the extended reliable fields follow the base header.

### 8.3 ReliableMessageID
Each reliable packet has **one** ReliableMessageID.
If the game needs to send multiple reliable messages at once, they must be bundled as one logical message or sent in separate reliable packets.

Rationale:
- Keeps packets much simpler.
- Avoids per-message headers.

`ReliableMessageID` is a strictly increasing 32-bit counter per endpoint.

### 8.4 Ordering Groups

#### 8.4.1 Definition
An ordering group defines an independent *reliable ordered stream*.  
Each group preserves message order *within* that group, but groups are independent from each other.

#### 8.4.2 Group semantics
- **Group 0** = unordered reliable message  
  Applied immediately when received.

- **Group 1..255** = ordered streams  
  Each group maintains:
  - `next_expected_reliable_id`
  - a small buffer of out-of-order reliable messages

#### 8.4.3 Purpose
Without groups, reliable messages behave like TCP:
- one lost message blocks all later reliable messages
- even if those later messages are unrelated

With ordering groups:
- inventory updates can be ordered within their stream
- ability activation messages can be ordered in another stream
- chat messages can be ordered separately
- losing a message in one stream does **not** block the others

This avoids global head-of-line blocking.

#### 8.4.4 Receiving a reliable message
1. If `order_group == 0`  
   -> deliver immediately, ack it

2. If `reliable_id == next_expected_id`  
   -> deliver  
   -> increment `next_expected_id`  
   -> then flush any buffered consecutive messages

3. If `reliable_id > next_expected_id`  
   -> store in buffer
   -> wait for missing messages

4. If `reliable_id < next_expected_id`  
   -> duplicate  
   -> ignore

### 8.5 Acknowledgment of Reliable Messages
Packet-level ACKs confirm only packet delivery—not reliable messages.  
So reliable messages require their own acknowledgement system.

#### 8.5.1 Reliable ACK Message
A Reliable ACK is a small control message to ensure data transfer:
```
type = RELIABLE_ACK
acked_reliable_id (32 bits)
order_group (8 bits)
```

#### 8.5.2 When to send a Reliable ACK:
- As soon as a new reliable message is received.
- When a buffered reliable message becomes deliverable.
- When an out-of-order message fills a gap.

#### 8.5.3 What is NOT acknowledged:
- Duplicates
- Messages already acknowledged
- Messages that belong to a group and arrive early but aren't yet deliverable

#### 8.5.4 Exemple

```
CLIENT (sender)                                                SERVER (receiver)
---------------------------------------+----------------------------------------
                                       |
+------------------------------------->+
SEND ReliableMessage(ID=42)            |
  Packet contains:                     |
  | type = RELIABLE_MESSAGE_PRESENT    |
  | order_group = 3                    |
  | reliable_id = 42                   |
                                       |
Sender stores message 42 in            |
retransmission queue                   |
                                       |
                                       |
                                       +--------------------------------------->
                                       |    RECEIVE ReliableMessage(ID=42)
                                       |      Check duplicate?
                                       |      Check ordering group?
                                       |      Buffer or deliver?
                                       |        (Assume new + in-order)
                                       |
                                       |
                                       +<--------------------------------------+
                                       |    GENERATE Reliable ACK (ID=42)
                                       |      Packet contains:
                                       |      | type = RELIABLE_ACK
                                       |      | acked_reliable_id = 42
                                       |      | order_group = 3
                                       |
                                       |
<--------------------------------------+
RECEIVE Reliable ACK (ID=42)           |
                                       |
Sender removes ID=42 from              |
retransmission queue                   |
                                       |
---------------------------------------+----------------------------------------
                                       |
--- LOSS CASE ---                      |
                                       |
If no ACK is received by timeout:      |
    timeout = max(1.25 * RTT, 20 ms)   |
                                       |
+------------------------------------->+
SEND ReliableMessage(ID=42)            |
  ...                                  |
                                       +--------------------------------------->
                                       |    RECEIVE ReliableMessage(ID=32)
                                       |      reliable_id 42 already received
                                       |      duplicate -> discard
                                       |
                                       |
                                       +<--------------------------------------+
                                       |    GENERATE Reliable ACK (ID=42)
                                       |      ...
---------------------------------------+----------------------------------------
```

## 8.6 Retransmission Logic

Each sent reliable message is stored in the **retransmission queue**:

```
struct RetransmitEntry {
    uint32_t reliable_id;
    uint8_t order_group;
    uint64_t last_send_time;
    std::vector<uint8_t> payload;
};
```

Reliable messages are removed once acknowledged.

Timeout Formula:
```
timeout = max(1.25 * RTT, 20 ms)
```

- 20 ms minimum ensures very responsive retransmissions.
- 1.25 RTT multiplier avoids false retransmits under jitter.

Packets carrying reliable messages are resent when timeout expires.

## 9. Client-Side Interpolation and Prediction

### 9.1 Interpolation
Clients MAY buffer world snapshots for a short duration (typically 50-100 ms).  
Rendering the world slightly in the past smooths over late or missing packets.

### 9.2 Prediction
Clients MAY predict local entity movement (e.g., the player's own character) using last known velocity and input.  
When authoritative state arrives, clients SHOULD correct predicted state using smooth reconciliation to avoid visual artifacts.

## 10. Server-Side State Synchronization
The server sends periodic world snapshots (e.g., every tick).  
The snapshot SHOULD include the minimal set of state necessary for clients to reconstruct the game world.

To reduce bandwidth, servers MAY:
- Compress snapshots
- Delta-encode states relative to the previous acknowledged snapshot
- Send per-client relevance-filtered data

## 11. Security Considerations
- UDP source addresses may be spoofed; authentication SHOULD be used.
- Malformed packets MUST be discarded.
- Sequence numbers SHOULD NOT wrap within a short session.

## 12. Example Message Flow
```
Client                         Server
   | ---- Input #100 ------->    |
   | <--- Snapshot #200 -------  |
   | ---- Input #101 ------->    |
   | <--- Snapshot #201 (lost)   |
   | <--- Snapshot #202 -------  |
```

Client infers loss of Snapshot #201 via sequence gap and continues rendering via interpolation.
