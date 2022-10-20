DST_DIR=/home/user/ping

sshpass -p "password" \
rsync -zar -e "ssh -p 2222 " \
--exclude=.svn \
--exclude=.cvs \
--exclude=.idea \
--exclude=.DS_Store \
--exclude=.git \
--exclude=.hg \
--exclude=*.hprof \
--exclude=*.pyc \
--exclude=../cmake-build-* \
--exclude=cp_files.sh \
--exclude=Dockerfile \
--exclude=setup_docker.sh \
--exclude=.gitignore \
./ root@127.0.0.1://${DST_DIR} --delete
