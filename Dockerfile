FROM python:3.12

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    wget \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-linux-x86_64.sh \
    && chmod +x cmake-3.28.3-linux-x86_64.sh \
    && ./cmake-3.28.3-linux-x86_64.sh --skip-license --prefix=/usr/local \
    && rm cmake-3.28.3-linux-x86_64.sh

ENV PYTHONUNBUFFERED=1

COPY requirements.txt .
RUN pip install --upgrade pip
RUN pip install -r requirements.txt

COPY . .

RUN mkdir -p build && cd build && cmake .. && make && cd .. && cp build/connect4 .

EXPOSE $PORT

CMD ["python", "app.py"]