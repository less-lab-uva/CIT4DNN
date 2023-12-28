FROM python:3.8
RUN python3 --version
RUN pip3 --version

RUN apt-get update && apt-get install gcc
RUN apt-get update && apt-get install -y python3-opencv
RUN pip3 install --upgrade pip

WORKDIR /cit4dnn

COPY requirements.txt ./requirements.txt
RUN pip3 install -r requirements.txt

COPY ./ ./

CMD ["./run_mnist.sh"]
