# usage ./push.sh commit_message branch name 

make clean

git add .
git commit -m $1
git push origin $2