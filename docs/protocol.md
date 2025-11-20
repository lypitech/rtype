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

This section defines the updated header format incorporating timestamp echo support.

### 5.1 Packet Header
```
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Type           |       Reserved        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Sequence                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Ack                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Ack Bits                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  Timestamp Echo                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  Ack Delay (µs)                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

Example in c:
```c
struct PacketHeader {
    uint16_t type;
    uint16_t reserved; // unused
    uint32_t seq;
    uint32_t ack;
    uint32_t ack_bits;
    uint32_t timestamp;
    uint32_t ack_delay;
};
```

### 5.2 Header Fields
- **Type (16 bits)** - Identifies the packet type (snapshot, input, reliable event, etc.).
- **Reserved (16 bits)** - Unused (padding)
- **Sequence (32 bits)** - Monotonically increasing packet identifier.
- **Ack (32 bits)** - Highest sequence number received from the peer.
- **Ack Bits (32 bits)** - Bitmask where bit *n* indicates receipt of (Ack - n - 1).
- **Timestamp Echo (32 bits)** - The sender echoes the timestamp (in ms) attached to the last acknowledged packet received from the peer.
- **Ack Delay (32 bits)** - Time in ms between the moment the acknowledged packet was received and the moment this acknowledgment was generated.

Total is a 24 bytes header.  
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
This layer provides selective reliability for game-relevant messages such as inventory changes, ability activations, or session events.

### 8.1 Identification of Reliable Messages
Reliable messages MUST be explicitly marked.  
Two mechanisms are defined:
1. **Packet Type Flag** - certain packet types are inherently reliable.
2. **Reliable Message Header** - individual messages inside a packet may carry a `ReliableID` field.

A reliable message SHALL contain:
- A unique 32-bit **Reliable Message ID**
- Optional ordering group identifier (0 = no ordering required)

### 8.2 Reliable Message Queue
Each endpoint maintains a retransmission queue containing:
- Reliable Message ID
- Associated packet sequence number
- Timestamp of last transmission
- Serialized payload

When a packet containing reliable messages becomes acknowledged (via Ack/AckBits), all corresponding messages MUST be removed from the retransmission queue.

### 8.3 Retransmission Logic
Reliable messages are retransmitted if not acknowledged within a timeout.

The recommended timeout is:  
**timeout = max(2 * RTT, 30 ms)**

This value is short enough for responsive gameplay but long enough to avoid excessive retransmissions.

### 8.4 Ordering Rules
If a reliable message belongs to an ordering group:
- Messages MUST be applied in increasing ReliableMessageID order.
- Out-of-order messages MUST be buffered until predecessors arrive.

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
