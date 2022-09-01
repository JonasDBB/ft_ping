# /bin/zsh

if docker image ls | grep -q "clion/remote-c-env"; then
	echo image found...
else
	docker build -t clion/remote-c-env:0.5 -f Dockerfile ~/workspace/Projects/ft_ping
fi

if docker container ls | grep -q "clion/remote-c-env"; then
	echo container already running...
else
	docker run -d --rm --cap-add sys_ptrace -p127.0.0.1:2222:22 --name clion_remote_env clion/remote-c-env:0.5
fi
