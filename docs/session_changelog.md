# Session Changelog
**Date:** 2025-12-09
**Focus:** Enhancing UI, Adding OHC Integration, and WiFi Attack Tools

## 1. Dashboard Improvements
- **OnlineHashCrack Stats Integration**:
  - Added a new status card in the "Config" section.
  - Displays: Monthly Usage (Mocked), Tasks count, Reset Date, and Masked API Key.
  - Logic added to populate this data from (simulated) API or localStorage.

- **Handshake Cracker Enhancements**:
  - Modified the "Password Cracker" card in the Handshakes view.
  - Added a `<select>` dropdown populated with captured handshakes.
  - Added a "📤 Enviar para OHC" button.
  - Implemented logic (`uploadSelectedHandshakeToOHC`) to send the selected handshake to OnlineHashCrack (requires setup).

## 2. WiFi Attack Tools
- **HCXDump Attack (Hunter Mode)**:
  - Added a new card in the `WiFi > Attacks` section.
  - Features: "Active Beaconing" toggle, Channel selection (All/1/6/11).
  - Button: "🚀 INICIAR CAÇADA" which triggers the `wifi_hcxdump` command.

## 3. Bluetooth Updates
- **Rename Button**: Changed button text from "Rename" to "Renomear" in the Chaos section for Portuguese localization consistency.

## 4. UI/UX Tweaks
- **Mascot Phrases**: Updated `mascots.js` to include "zzZ... WiFi... zzZ..." for idle states and more realistic technical jargon for active states.
- **Config Section**: Cleaned up the OnlineHashCrack setup card layout.

## 5. Technical Changes
- **index.html**:
  - Extensive modifications to add new cards (OHC, HCXDump, Scheduler, Sounds).
  - Fixed HTML structure for **Bluetooth Chaos** card to ensure it sits correctly inside the Bluetooth view (hidden when not active).
- **effects.css**:
  - Updated `.portal-transition` to use `var(--primary)` so the animation color matches the active theme.
- **JS Logic**:
  - Added `startHCXDump()`.
  - Added `updateOHCStats()`.
  - Added `scheduleHandshakeReminder()` & `saveHandshakeSchedule()`.
  - Added `uploadSoundToSD()`.
