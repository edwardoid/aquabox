# ![](https://raw.githubusercontent.com/edwardoid/aquabox-frontend/master/resources/package.svg =50x50) AquaBox System 	![](https://raw.githubusercontent.com/edwardoid/aquabox-frontend/master/resources/icon.png =50x50)

AquaBox is a cloud based system for controlling aquarium equipment.


## Building
Right now the cloud service and client service can be run on **armv6**, **armv7** and **x86_64** platforms. The server is designed to be run on a cloud machine with x86_64 architecture, but should work on both armv6 and armv7 as well. Client is supposed to be run single-board computers with armv6/armv7 architecture.
The client service is tested on:
**armv7** -- [Orange Pi Zero](http://www.orangepi.org/orangepizero/) with Armbian distro based on Debian **Buster**. See releases [here](https://www.armbian.com/orange-pi-zero/#kernels-archive-all)
**armv6** -- [Raspberry Pi Zero W](https://www.raspberrypi.org/products/raspberry-pi-zero-w/) with DietPi([details](https://dietpi.com/phpbb/viewtopic.php?t=6104)) based on Debian Buster. Download image [here](https://dietpi.com/downloads/images/DietPi_RPi-ARMv6-Buster.7z)

The server service tested on a **Ubuntu 20.04** machine

### Requirements
In order to build the project you need: 
 - Linux (Tested on Ubuntu 20.04, but should work on any distro)
 - Docker (Tested on version 20.10.7, build f0df350)
 - git (Tested on version 2.25.1)
 - SSH client
 - scp client
 - sshpass

### Build process
#### Getting the sources
Fetch latest sources
	$ git clone http://github.com/edwardoid/aquabox.git
	$ cd aquabox
	$ git submodule update --init --recursive
#### Building images
Just run:

	$ ./Docker/build_for_armv6.sh # To build for armv6 
	$ ./Docker/build_for_armv7.sh # To build for armv7
	$ ./Docker/build_for_x86_86.sh # To build for x86_64
#### Development
To have preconfigured environment for comfortable development
just after getting sources use following script(s):

    ./Docker/run_armv6.sh
    ./Docker/run_armv7.sh
    ./Docker/run_x86_64.sh

Wait for Docker to build container and execute following script **inside** of the container

		$ source /sources/Docker/build.sh


**Remember the command above will clean all previous build and start building everything from scratch. Only downloaded/built Conan packages will stay!** In order to continue build if something went wrong **inside** the Docker container following commands/functions can be used:

		$ prepare_build # To build Conan packages and run CMake
		$ do_build # Run make. After this step you will have both binaries in /build/bin
		$ pack_client # Create an archive with client executable and its libraries
		$ pack_server # Create an archive with cloud server executable and its libraries

After building and creating packages following files will be created:

 - **SOURCE_DIR**/build/**PLATFORM**/aquabox-client-x86_64_**COMMIT_HASH**_**TIMESTAMP**.tar.gz
  - **SOURCE_DIR**/build/**PLATFORM**/aquabox-client-latest.tar.gz
   - **SOURCE_DIR**/build/**PLATFORM**/aquabox-server-x86_64_**COMMIT_HASH**_**TIMESTAMP**.tar.gz
   - **SOURCE_DIR**/build/**PLATFORM**/aquabox-server-latest.tar.gz

### Deploying client service to a target machine
In order to deploy the package following steps should be done on 

	$ cd ./build/<PLATFORM>
	$ TARGET_IP=192.168.1.2 # Replace with ip of you Orange Pi Zero or Raspberry Pi Zero W
	$ ROOT_PASS=root # Replace with root password on target system
	$ ./../../scripts/push_update.sh $TARGET_IP $ROOT_PASS
This will upload `aquabox-client-latest.tar.gz` to target device and install/replace old instance

### Running client service in Docker
Client service(build with x86_64 platform) can be run in Docker image, but some functionality needs to be disabled

	$ cd /build/bin
	$ export DBUS_SYSTEM_BUS_ADDRESS=unix:path=/var/run/dbus/system_bus_socket
	$ ./aquabox-client --nodraw \
	$                  --ignore-networking \ # If you does not have NetworkManager/WiFi network adapter
	$                  --disable-access-point \ # To keep you connected to the Internet
	$                  --disable-mesh	# Probably you don't have SPI overlay enabled on your machine
	$ # See all options: ./aquabox-client --help
	
### Running cloud service in Docker
Actually there is no specific steps.

	$ cd ./build/<PLATFORM>
	$ tar xf aquabox-server-latest.tar.gz -C <DIRECTORY_YOU_WANT>
	$ cd <DIRECTORY_YOU_WANT>/bin
	$ export LD_LIBRARY_PATH=$PWD/../lib
	$ ./aquabox-server

Steps if you run cloud service in the Docker image (`$ ./Docker/run_x86_64.sh'):

	$ cd /build/bin
	$ ./aquabox-server
	
## Prebuild binaries
Latest images for cloud and client services can be found here:
http://aquabox.me:8081/armv6
http://aquabox.me:8081/armv7
http://aquabox.me:8081/x86_64

See version information in `manifest.json` located in the downloaded archive.