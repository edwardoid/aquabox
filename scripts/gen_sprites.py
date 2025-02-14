#!/bin/python

import urllib.request
import os
from cairosvg import svg2png

class Entry:
    def __init__(self, name, remote):
        self.name = name
        self.remote = remote

    name = ""
    remote = ""
    local = ""

entries = [
    Entry("wifi0", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/src/device/signal_wifi_0_bar/materialiconsoutlined/24px.svg"),
    Entry("wifi1", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/src/device/signal_wifi_1_bar/materialiconstwotone/24px.svg"),
    Entry("wifi2", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/src/device/signal_wifi_2_bar/materialiconstwotone/24px.svg"),
    Entry("wifi3", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/src/device/signal_wifi_3_bar/materialiconstwotone/24px.svg"),
    Entry("wifi4", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/src/device/signal_wifi_4_bar/materialiconstwotone/24px.svg"),
    Entry("cloud_on", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/symbols/web/cloud/materialsymbolsoutlined/cloud_24px.svg"),
    Entry("cloud_off", "https://raw.githubusercontent.com/google/material-design-icons/refs/heads/master/symbols/web/cloud_off/materialsymbolsoutlined/cloud_off_20px.svg")
]

for x in entries:
    print(x.name)
    base, ext = os.path.splitext(x.remote)
    print(ext)
    urllib.request.urlretrieve(x.remote, x.name + ext)