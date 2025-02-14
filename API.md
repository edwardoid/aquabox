# API


## Client <-> Cloud WS API


Client initiates Web Socket connection to:

```
ws://<target_adress>:<stream_port>/api/v1/<serial>/<application_identifier>/rpc
```
Where
* `<target_adress>` is the cloud address. E.g. `aquabox.me`
* `<stream_port>` is the WebSocket port. E.g. `9182`
* `<serial>` is a unique serial number of a target AquaBox controller. E.g. `testClient1`
* `<application_identifier>` is a unique identifier allocated by cloud for the particular client application.

Example:

`ws://aquabox.me:9182/api/v1/testClientq1/testClienta1/rpc`

Client can send requests/commands via WebSocket, and each request **must** contain following structure:

```json
{
    "command" : "<command>",
    "boxId" : "<serial>",
    "rqid": 1234,
    "data": {
        ...
    },
    "params": {
        ...
    }
}
```

Where

`<command>` name of the command to be performed:

  * `get_status` Return status of the Aquabox
  * `get_all_devices` Return list of attached devices and sensors attached to the controller
  * `get_device` Return information about device
  * `update_device`
  * `get_rules`
  * `get_rule`
  * `create_rule`
  * `update_rule`
  * `delete_rule`
  * `get_device_rules`
  * `scan`
  * `networks`
  * `connect`
  * `set`
  * `subscribe_to_box`

`<serial>` is a unique serial number of a target AquaBox controller. E.g. `testClient1`

`rqid` unique id of the request.

`data` Data associated with the request

`params` Params for the request

### Get all devices by `get_all_devices`

Example:

```json
{
    "command" : "get_all_devices",
    "boxId" : "testClient1",
    "rqid": 1234
}
```

`data` in response object contails list of [device objects](#device-object):

```json
{
  "boxId": "testClient1",
  "command": "get_all_devices",
  "data": {
    "devices": [
      {
        "accepted": false,
        "address": "PHY:MOCK_GPIO:18",
        "available": true,
        "class": "relay",
        "id": "vdev1",
        "index": 0,
        "meta": {
          "model": "100W",
          "vendor": "ada"
        },
        "name": "virtual 23",
        "on": 0,
        "rules_enabled": false,
        "type": "generic"
      }
    ]
  },
  "params": {},
  "rpid": 1234,
  "type": "command"
}
```

### Get Device by `get_device`

To get a specific device a value for `":dev"` in `"params"` should be provided.

Example request:

```json
{
  "command": "get_device",
  "boxId": "testClient1",
  "rqid": 1213,
  "params": {
    ":dev": "vdev1"
  }
}
```

Example response:

```json
{
  "boxId": "testClient1",
  "command": "get_device",
  "data": {
    "accepted": true,
    "address": "PHY:MOCK_GPIO:18",
    "available": true,
    "class": "relay",
    "id": "vdev1",
    "index": 0,
    "meta": {
      "model": "100W",
      "vendor": "ada"
    },
    "name": "virtual 2",
    "on": 0,
    "rules_enabled": true,
    "type": "heater"
  },
  "params": {
    ":dev": "vdev1"
  },
  "rpid": 1213,
  "type": "command"
}
```

### Updating a device

## Device object

```json
    {
        "accepted": boolean, // True if device has been attached to the controller box
        "address": string, // Read-only property, indicating of way the cdevice is connected to controller
        "available": boolean, // True if device is attached and active
        "class": string, // See "Device classes"
        "id": string, // Unique identifier of the device
        "index": number, // Prefferen position is list of devices
        "meta": { // Meta information abotu device, contains additional information associated with it
            "model": string,
            "vendor": string
        },
        "name": string, // User friendly name
        "rules_enabled": boolean, // True if device is managed by it's rules
        "type": string // See Device types
    }
```

Example:
```json
      {
        "accepted": false,
        "address": "PHY:MOCK_GPIO:18",
        "available": true,
        "class": "relay",
        "id": "vdev1",
        "index": 0,
        "meta": {
          "model": "100W",
          "vendor": "ada"
        },
        "name": "virtual 23",
        "on": 0,
        "rules_enabled": false,
        "type": "generic"
      }
```

### Device classes

Here is list of posible device classes: analog_relay, relay, step, unknown.
See `system\src\types\device_type.cpp` for more.

### Device types

Here is list of currently possible device types: "generic", "air_pump", "water_pump", "internal_filer", "external_filter", "sump", etc.
See `system\src\types\device_type.cpp` for more