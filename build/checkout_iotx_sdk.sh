origin_path="$(pwd)"
cd framework/protocol/linkkit/sdk

if [ ! -d "iotx-sdk-c_clone" ]; then
    git clone https://github.com/aliyun/iotkit-embedded.git iotx-sdk-c_clone
    if [ $? -ne 0 ]; then
        echo "clone iotx-sdk-c fail!"
        exit 1
    fi
    cd iotx-sdk-c_clone
    	git checkout -b rel_ada --track origin/rel_ada
    if [ $? -ne 0 ]; then
        echo "checkout linkplatform_refactor branch fail!"
        exit 1
    fi

fi
