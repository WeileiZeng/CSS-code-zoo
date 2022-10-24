# reasults on searching good CSS codes

n=20, G_x_row=9, G_z_row=9, d=[2,5],[4,2]
n=22,         9          9    [4,3]
  22          10         10   [3,5]


Renyu's example:
n0=11  [ 0, 3 ] [0,1]   n=22 dX=4 dZ=5 d0=4

# how-to-use-weilei-lib
A sample repo on how to use functions in weilei_lib. One can fork this one and use it as a starting point


# How to use from this repo

I use weilei_lib as a submodule here. It works like two repo, but just one inside the other. One should commit changes independently. To clone it, use the following cmd.
```
git clone --recurse-submodules https://github.com/QEC-pages/how-to-use-weilei-lib
```
You can test it by
```
make test_lib
make test
```




# info on sub module

see https://git-scm.com/book/en/v2/Git-Tools-Submodules

init:
```
git submodule add git@github.com:WeileiZeng/weilei_lib.git
```
You can also use http instead of ssh

Then you work on those like two different repo.
