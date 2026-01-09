# Remaining Tasks & Future Work
**Last Updated:** 2025-12-09

## Priority TODOs
1.  **Backend Integration (C++ / ESP32)**
    *   **HCXDump Command**: The `wifi_hcxdump` command needs to be implemented in `wifi_attacks.cpp` to actually run the attack logic (channel hopping + active beaconing).
    *   **OHC API Proxy**: To make "OnlineHashCrack" stats real, the backend needs to proxy requests to `https://www.onlinehashcrack.com/api` (since browser CORS might block direct access from the dashboard).
    *   **Bluetooth Chaos Scan**: Ensure `ble_chaos_scan` returns the exact JSON format expected by the new UI table.

2.  **Handshake Management**
    *   **Sync**: Ensure the `/api/handshakes` endpoint returns `id`, `ssid`, type, etc., correctly for the new Select box to populate.
    *   **Delete**: Add a delete button next to the select box for better management.

3.  **Refactoring**
    *   **Separation of Concerns**: `index.html` is getting very large (2500+ lines). Consider moving the new inline JS to `js/app.js` or `js/pages/attacks.js`.

## Wishlist / Ideas
- **Dark Mode Toggle**: Currently hardcoded to Cyberpunk/Dark themes. A light mode toggle in config might be useful (low priority).
- **Multi-Language Support**: Complete localization (PT-BR is default, but code has mixed EN/PT comments and some UI labels).
- **Real-time Terminal**: Improve the WebSocket terminal to show `hcxdumptool` output in real-time.
