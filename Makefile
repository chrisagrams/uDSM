.PHONY: all vdso clean docker-img docker-build dep

all:
	make -C criu-3.15 -j$(shell nproc)
	make -C criu-3.15/dsm_client
	make -C tools

vdso:
	$(shell ./vdso/vdso.sh)

clean:
	make -C criu-3.15 clean
	make -C tools clean

dep:
	sudo apt-get update -y && sudo apt install -y --no-install-recommends \
        libprotobuf-dev libprotobuf-c-dev protobuf-c-compiler \
        protobuf-compiler python3-protobuf \
        libcap-dev libnl-3-dev libnet-dev libbsd-dev pkg-config \
        python3-future libnet1-dev libnftables-dev libgnutls28-dev \
        gcc make

docker-img:
	docker build -t ubuntu2204:criu .

docker-build:
	docker run --rm \
		-v $(PWD)/criu-3.15:/code \
		--user "$$(id -u):$$(id -g)" \
		ubuntu2204:criu make -C /code
