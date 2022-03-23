# The image to start out with
FROM ubuntu:20.04
# Specify the user
USER root
# run some commands
RUN apt update && apt install -y \
    vim \
    curl \
    make \
    gcc \
    build-essential \
    libssl-dev \
    git \
    libmicrohttpd-dev \
    libpq-dev
# Copy something into the image
ADD "source" /home/newuser/

RUN git clone https://github.com/eclipse/paho.mqtt.c.git
WORKDIR "paho.mqtt.c"
RUN make
RUN make install
WORKDIR "/home/newuser/"
RUN make
CMD ["./Pro_subscriber"]
