mkdir /tmp/dockercopy
rsync -avzh ~/workspace/Projects/ft_ping/ /tmp/dockercopy/ \
 --exclude cmake-build-*/ \
 --exclude .git/ \
 --exclude .idea/ \
 --exclude Dockerfile \
 --exclude .gitignore \
 --exclude setup_docker.sh \
 --exclude cp_files.sh
docker cp /tmp/dockercopy/ clion_remote_env:/home/user/ping/
rm -rf /tmp/dockercopy