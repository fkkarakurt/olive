
# Olive - Web Server

Olive is a web server developed in C that supports static/dynamic web pages from the ground up.



Web servers communicate with clients (i.e. browsers) using the HTTP protocol, which in turn is based on the TCP/IP protocol. The TCP communication interface provided by the Linux system was used to implement the HTTP protocol in Olive.
## Run it on your computer

Clone the Project after Fork:

```bash
  git clone https://github.com/fkkarakurt/olive
```

Go to the project directory:

```bash
  cd olive
```

Install Requirements:

```bash
  sudo apt-get update -y
  sudo apt-get install -y libssl-dev
```

Build the project by running the following command in the `olive/` folder:

```bash
make && cd cgi/ && make && cd ..
```

Then grant the necessary permissions:
```bash
chmod +x olive
chmod +x cgi/run
```

Use the following command to start Olive:
```bash
./olive <port>

# Example:
# ./olive 1212
```

You will get both static and dynamic pages.

Address of your static page: `localhost:1212`
Address of you dynamic page: `localhost:1212/cgi/run?5&6`

*Make sure the addition in your dynamic page is consistent with the numbers you specify in the URL.*
