<img src="https://media.giphy.com/media/WUlplcMpOCEmTGBtBW/giphy.gif" width="100">

<img src="https://media2.giphy.com/media/ZEUODEtQiUZWGg6IHR/giphy.gif" width="100">
<img src="https://i.giphy.com/media/jNNUsQaAx0myWAXw1h/giphy.webp" width="100">

# Course project system programming

[![Typing SVG](https://readme-typing-svg.demolab.com?font=Fira+Code&pause=1000&color=0FF74B&width=435&lines=Реализация+хранилища+с+дискреционным+контролем+доступа)](https://git.io/typing-svg)

## Complie
#### 1. Modify files ```CMakeLists.txt``` path to your ```vcpkg``` 
#### 2. Install ```nlohmann-json``` by ```vcpkg```
#### 3. Move config file ```config.cfg``` to  ```/usr/local/bin/config.cfg ``` and ```listening_daemon.service``` to ```/etc/systemd/system```
#### 4. Install library in ```/ServerSide/IncludeMake/build```:
```
cmake ..
sudo make install 
```
#### 5. Build server in ```/ServerSide/ListeningDaemon/build```:
```
cmake ..
make
```
#### 6. Build client-user in ```/Client/build```:
```
cmake ..
make
```

## Run
#### 1. Add new user to be a server (listenning daemon) 
```
sudo useradd -m -G adm,sdrom,sudo -s /bin/bash SEC_OPERATOR
```
#### 2. Run server by user ```SEC_OPERATOR```
```
sudo systemctl start listening_daemon.service
```
#### 3. Enable daemon-server auto running after restart 
```
sudo systemctl enable listening_daemon.service
```
