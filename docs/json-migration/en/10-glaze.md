# GLAZE

## 1. Summary
- Modern high-performance C++ serialization/reflection library.
- Conceptual mismatch for direct YAJL replacement in this repository.

## 2. Project status
- Repo: <https://github.com/stephenberry/glaze>
- Active project with frequent releases.

## 3. Technical model
- C++20+ oriented typed binding/reflection model.
- Not centered on YAJL-like C callback streaming.

## 4. Build/integration
- Usually easy as bundled header-only/CMake dependency.
- C++20 requirement may conflict with current C++17 baseline.

## 5. Fit to repository
- Better for typed DTO workflows than dynamic arbitrary JSON event flows.

## 6. Module vs package
- Typically bundled; system packaging less common.

## 7. Integration strategy
- If used, isolate to optional C++20 tool/subcomponent paths.

## 8. Migration effort
- **Very high** as a universal YAJL replacement.

## 9. Recommendation
- **Not recommended as primary backend**.
