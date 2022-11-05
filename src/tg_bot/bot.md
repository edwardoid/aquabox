# Aquabox Telegram Bot description


# Commands

Following commands should be supported:
* `/authorize` - This commands allows to add a new box to list of boxes to be controlled.

* `/list` - List all available boxes

* `/box <options>` - Perform some actions on the selected box
  + A number can be used as an option to select a specific box
  + `a|auth` - Generate authentification token (valid for 60min) for current box
  + `forget` - Forget current box
  + `subscrube` - Subscribe for updates
  + `unsubscribe` - Unsubscribe from updates

* `/maintenance <options>` - Maintenance mode
  + `<number><postfix>` - Explicitly specify maintenance duration. Available postfixes are `h|hr|hour|hours`, `m|min|minute|minutes`, `d|day|days`
  + `done` - Stop maintenance mode.

* `/remove <num>` - Remove `<num>`th box from list of all available devices

* `/devices` - List all available devices

* `/device <id|name>` - Select device to work with

* `/rules` - List all rules for current devices.

* `/rule <num> <operation>` change `<num>`th rule. Operations can be:
  + `e|erase|delete` - Remove the rule
  + `d|off|0|disable` - Disable the rule
  + `o|on|1|enable` - Enable the rule

* `/help` - Show commands reference

✨ Planned but for later implementation:

* `/algae <option>`
  + `r|report` - Report an algae problem in the aquarium
* `/inspire` - Ask for an insiparion material from web.
* `/search <text>` - Perform a quick search in internal KB about algae, fish, technology, etc.

⚠️ Following command(s) are for debug purpose only and will be restricted or removed at close-to-release state:

* `/debug <options>` - Display debug information. Options can be:
  + `uptime` - Show system uptime
  + `server_uptime` - Show server uptime
  + `restart` - Restart the box
  + `dmesg` - Show dmesg
  + `gpio` - Show gpio states
  + `state` - My Telegram session state
  + `ip` - Get nework IP to connect too the host.
  
