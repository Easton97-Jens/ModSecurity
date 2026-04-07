# cJSON

## 1. Summary
- Lightweight and popular, but narrower feature model.
- Limited fit as a full YAJL replacement.

## 2. Project status
- Repo: <https://github.com/DaveGamble/cJSON>
- Active project; release v1.7.19 (2025-09-09).

## 3. Technical model
- C DOM-centric API (`cJSON*`) with parse/print functions.
- Not centered around robust SAX/incremental callbacks.

## 4. Build/integration
- Easy to bundle; distro packaging exists but varies.

## 5. Fit to repository
- Usable for test DOM and simple writer paths.
- Weak fit for existing incremental callback parser architecture.

## 6. Module vs package
- Bundled optional mode is practical due to size; system still preferred where available.

## 7. Integration strategy
- Keep as optional lightweight backend only.

## 8. Migration effort
- **High** for complete parity.

## 9. Recommendation
- **Experimental/optional only**.
