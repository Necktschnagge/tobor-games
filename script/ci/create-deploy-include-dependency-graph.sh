#!/bin/bash
pushd .

########################### ENVIRONMENT VARIABLES #####################################
echo "Determine environment variables..."

#environment variables:
git_repo_url=$(git remote get-url origin)
echo -e "\tgit_repo_url: ${git_repo_url}"
user_repo_id=$(echo "${git_repo_url}" | sed -E 's/https:\/\/\w*.\w*\///' | sed -E 's/\.git//')
echo -e "\tuser_repo_id: ${user_repo_id}"
pull_id=${3}
echo -e "\tpull_id: ${pull_id}"
git_pull_url="${git_repo_url}/pull/${pull_id}"
echo -e "\tgit_pull_url: ${git_pull_url}"

#config variables:
diagram="dependency-diagram" #create diagram if present
links="no-links" #do not post a link to diagram if present
git_branch_artifacts="artifacts"
git_base_branch_for_artifacts="main"

#secret variables:
git_username=${1}
git_access_token=${2}

########################### CHECK FOR PULL REQUEST #####################################
echo
echo "Check if this is a pull request:"
if [ "${pull_id}" -eq "${pull_id}" ] 2>/dev/null; then #always use single "[]" here so that "-eq" requires integers
	echo -e "\tThis is a pull request build. Script will continue to create dependency diagram."
	echo -e "\tThis build is for pull request #${pull_id}."
	echo -e "\tAlso see: ${git_repo_url}/pull/${pull_id}"
else
	echo -e "\tThis is not a pull request build. Aborted."
	popd
	exit 0
fi

########################### PULL REQUEST ONLY VARIABLES #####################################

#environment variables:
#echo "+++ Graph +++"
#git log --graph --all
#echo "+++++++++++++"

#there is only one commit fetched into local repository !!!!!!

git_hash_last_commit=${4} # $(git rev-parse HEAD^2) # CI merges the current feature branch commit into pull request base branch. Therefore on CI there is always one commit more. -> use ^2 for merge ancestor (see https://medium.com/@gabicle/git-ancestry-references-explained-bd3a84a0b821)
echo -e "\tgit_hash_last_commit: ${git_hash_last_commit}" #last commit on feature branch

#config variables:
git_branch_for_ci_job="branch-ci-${git_hash_last_commit}"
artifact_path="artifacts/diagram/${git_hash_last_commit}.pdf"

########################### CENTRAL EXIT POINT #####################################
git switch -c ${git_branch_for_ci_job} #switch to new branch pointing to current HEAD
quit(){
	echo "Exiting script with code ${1}"
	popd
	echo -e "\tTidy up working directory..."
	git add -u && git add *
	git status
	git reset --hard
	git checkout ${git_branch_for_ci_job}
	echo -e "\tTidy up working directory   ...DONE!"
	exit ${1}
}

########################### CHECK PULL REQUEST LABELS #####################################
echo
echo "Checking labels of pull request ${git_pull_url}..."
git_pull_labels=$(curl -H "Authorization: token ${git_access_token}" -X GET "https://api.github.com/repos/${user_repo_id}/issues/${pull_id}/labels")
echo -e "\tgit_pull_labels: ${git_pull_labels}"
if [[ ${git_pull_labels} =~ ^.*${diagram}.*$ ]]; then
	echo -e "\tFound label >${diagram}<. Continuing..."
else
	echo -e "\tDid not find label >${diagram}<. Abort diagram creation."
	quit 0
fi

########################### INSTALL DEPENDENCIES #####################################
echo
echo "Install dependencies..."
sudo apt-get install -y graphviz

########################### CREATE DIAGRAM #####################################
echo
echo "Create diagram..."
cd ./script
./udg-unix-create-include-dependency-graph.sh
cd ..

########################### UPLOAD ARTIFACT #####################################
echo "Start uploading pdf build artifact..."
LEFT_TRIES=10
while true; do
	echo -e "\tCheckout branch ${git_branch_artifacts}"
	(git fetch https://${git_username}:${git_access_token}@github.com/${user_repo_id} && git checkout ${git_branch_artifacts}) || quit 4
	echo -e "\tgit merge origin/${git_base_branch_for_artifacts}"
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" merge origin/${git_base_branch_for_artifacts} || quit 5 # this is possibly concurrent to another job creating the same merge commit.
	echo -e "\tCopy diagram PDF"
	mkdir -p ./artifacts/diagram
	cp ./build/include-dependency-graph/src-all.pdf "${artifact_path}" # if the file is already present, cp overwrites the old one.
	echo -e "\tgit add -f ${artifact_path}"
	git add -f "${artifact_path}"
	echo -e "\tgit status"
	git status
	echo -e "\tgit commit -m \"Automatic upload of dependency diagram PDF\""
	#we may skip telling email with "-c user.email=ci-for-necktschnagge@example.org"; not needed but than we get some ugly mail adress from Azure setup.
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "Automatic upload of dependency diagram PDF" # this is possibly concurrent to another job running this script.
	echo -e "\tgit status"
	git status
	echo -e "\tgit push" #this may fail after concurrent commits:
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${git_branch_artifacts} && break
	echo
	echo
	echo -e "\tPush was not successful. Trying again..."
	git checkout ${git_branch_for_ci_job}
	git branch -D artifacts
	sleep $(($LEFT_TRIES))s
	let LEFT_TRIES=LEFT_TRIES-1
	if [ $LEFT_TRIES -lt 1 ]; then
		echo -e "\tFAILED:   Uploading pdf build artifact."
		curl -H "Authorization: token ${git_access_token}" -X POST -d "{\"body\": \"ERROR: Failed to push preview artifact!\"}" "https://api.github.com/repos/${user_repo_id}/issues/${pull_id}/comments"
		quit 1
	fi
done

########################### POST COMMENT #####################################
echo -e "\tUploading PDF build artifact... DONE"
if [[ ${git_pull_labels} =~ ^.*${links}.*$ ]]; then
	echo "Found label ${links}. Skip posting a comment to the pull request linking to the preview PDF."
	quit 0
fi
echo "Posting comment into pull request..."
curl -H "Authorization: token ${git_access_token}" -X POST -d "{\"body\": \"See dependency diagram here: [${git_hash_last_commit}.pdf](https://github.com/${user_repo_id}/blob/artifacts/${artifact_path})\"}" "https://api.github.com/repos/${user_repo_id}/issues/${pull_id}/comments"	
quit 0;
