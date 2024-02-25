# rg35xx-misc
Misc stuff for rg35xx


## Sign commits

```
# set gpg format to ssh
% git config --global gpg.format ssh   

# set the key to sign  
% git config --global user.signingkey ~/.ssh/id_rsa.pub

# avoid to use "-S" in each commit
% git config --global commit.gpgsign true
```
Note: change `~/.ssh/id_rsa.pub` to your public key path

## Verify signers
```
# create allowed signers file
% touch ~/.ssh/allowed_signers                         

# set the path to the file in git
% git config gpg.ssh.allowedSignersFile ~/.ssh/allowed_signers  
 
# add the entry corresponding to our pub key 
# (change '~/.ssh/id_rsa.pub' as needed)
% echo "$(git config --get user.email) namespaces=\"git\" $(cat ~/.ssh/id_rsa.pub)" >> ~/.ssh/allowed_signers

# check the resulting entry, look for the e-mail we use for commits,
# followed by 'namespaces="git"' and our pub key
% cat ~/.ssh/allowed_signers 
```

Now, use `--show-signature` parameter when using `git log` to see the signatures:
```
% git log --graph --show-signature
```

Source: https://docs.gitlab.com/ee/user/project/repository/signed_commits/ssh.html
