# PagerDuty Shift Tracker

CLI tool to calculate on-call shift hours from PagerDuty schedules.

## Requirements

- g++ with C++20 support
- libcurl

## Setup

Place your config at `~/.config/pagerduty/pg-config`:

```json
{
  "client": {
    "user_id": "YOUR_USER_ID"
  },
  "api": {
    "key": "YOUR_API_KEY"
  }
}
```

## Build & Run

```
make run
```
