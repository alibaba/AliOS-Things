# CI_UT

If you want to use a specific branch in the UT project for CI, you need to modify the tools/ci/ut_branch.conf file.
Please refer to the notes of tools/ci/ut_branch.conf file for details.

# Define URI & BRANCH & PATH in ut_branch.conf according to the following format

[UT]
    BRANCH : master
    PATH   : ../adf-libs-ut

[ADF_LIBS]
    BRANCH : master
    PATH   : components/esp-adf-libs
