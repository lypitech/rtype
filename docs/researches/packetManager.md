# **Why Our R-Type Project Should Use Conan Instead of vcpkg or CPM.cmake**

## **Overview**
Our R-Type project depends on **raylib**, **nlohmannjson**, and **ASIO**, and needs a dependency manager that is reliable across platforms, works well in CMake, and keeps our builds predictable for every developer. After evaluating **Conan**, **vcpkg**, and **CPM.cmake**, Conan provides the strongest overall workflow for this type of project.

# **Conan: Key Advantages**

### **Reproducible Builds**
Conan uses **profiles** and **lockfiles** to ensure that all developers and CI pipelines use the same compiler settings, dependency versions, and configurations. This prevents inconsistent builds and avoids environment-related bugs.

### **Efficient CMake Integration**
Conan's CMake integration is straightforward and modern, allowing us to simply call `find_package()` for raylib, and ASIO without manual FetchContent setups or custom build scripts.

### **Binary Caching for Faster Builds**
Conan can download or locally build dependencies once and reuse them, reducing build times significantly-especially important for raylib.


# **Why Not vcpkg?**
Although vcpkg is popular and easy to use, it presents several issues for a project like R-Type:

### **1. Limited Flexibility**
vcpkg enforces configuration through *triplets*, which constrain how libraries are built. This makes custom tuning (static/shared options, compiler flags, experimental configs) more difficult compared to Conan's fully customizable profiles.

### **2. Rebuilds and Slow Installations**
vcpkg often recompiles dependencies when no prebuilt binaries exist for your platform or configuration. For libraries like raylib:

- Building from source frequently becomes slow
- CI build times degrade
- Developer onboarding takes longer

### **3. Global Toolchain Side Effects**
vcpkg uses a **global** toolchain file.
This means:

- All dependencies must come from vcpkg
- Mixing custom libraries with vcpkg libraries can become messy
- There is less isolation and version control compared to Conan's per-project dependency model

For multi-platform game development with varied environments, these constraints can cause friction.

# **Why Not CPM.cmake?**
CPM is lightweight but insufficient for a real project like R-Type.

### **1. Always Builds From Source**
CPM.cmake fetches dependencies using `FetchContent`.
This means:

- Every developer compiles raylib and ASIO from scratch
- No caching
- Slower builds
- No central binary management

This is a major drawback for projects with several compiled components.

### **2. No Version or ABI Management**
CPM has no:

- Lockfiles
- Compatibility checks
- Conflict resolution
- Reproducibility guarantees

If upstream repositories change, our builds change too-potentially breaking functionality without warning.

### **3. Not Scalable for Larger Projects**
While fine for header-only libraries or simple utilities, CPM becomes burdensome as soon as:

- Multiple platforms
- Multiple dependencies
- or CI pipelines are involved.  
  Game projects like R-Type quickly exceed CPM's design scope.

---

### **Conan Security Essentials**

#### Supply Chain
- Public remotes may serve compromised or tainted packages.
- Namespaces help but do not prevent malicious uploads or upstream compromises.

#### Recipe Execution
- `conanfile.py` executes arbitrary Python.
- Treat all third-party recipes as untrusted; build in isolated, non-privileged environments.

#### Integrity
- Verification is opt-in.
- Use manifests (`--manifests` / `--verify`), cache audits, and `upload --check`.

#### Remote Governance
- Restrict remotes (`--allowed-packages`, recipes-only).
- Regularly run `conan audit` for CVE detection.

#### Operational Practices
- Pin versions; use lockfiles.
- Review third-party recipes.
- Run with least privilege.
- Keep the Conan client updated.
