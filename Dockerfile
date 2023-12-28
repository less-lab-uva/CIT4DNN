FROM python:3.8

#FROM ubuntu:18.04


#RUN apt-get update
#RUN apt-get install -y software-properties-common
#RUN add-apt-repository ppa:deadsnakes/ppa
#RUN apt-get install -y python3.8 python3-pip
# Update symlink to point to latest
#RUN rm /usr/bin/python3 && ln -s /usr/bin/python3.8 /usr/bin/python3
RUN python3 --version
RUN pip3 --version

RUN apt-get update && apt-get install gcc
RUN apt-get update && apt-get install -y python3-opencv
RUN pip3 install --upgrade pip

WORKDIR /cit4dnn

COPY requirements.txt ./requirements.txt
#RUN pip3 install https://storage.googleapis.com/tensorflow/linux/cpu/tensorflow_cpu-2.3.0-cp38-cp38-manylinux2010_x86_64.whl
RUN pip3 install -r requirements.txt

COPY ./ ./

CMD ["./run_mnist.sh"]
