
VERSION=0.1

all: arm32v7_linux x86_linux simulation

arm32v7_linux: arm32v7_ubuntu_wpantund
	docker build -t openthread/wpantund_arm32v7_linux arm32v7_ubuntu_wpantund && \
	docker push openthread/wpantund_arm32v7_linux && \
	docker tag openthread/wpantund_arm32v7_linux openthread/wpantund_arm32v7_linux:$(VERSION) && \
	docker push openthread/wpantund_arm32v7_linux:$(VERSION)

x86_linux: x86_ubuntu_wpantund
	docker build -t openthread/wpantund_amd64_linux x86_ubuntu_wpantund && \
	docker push openthread/wpantund_amd64_linux && \
	docker tag openthread/wpantund_amd64_linux openthread/wpantund_amd64_linux:$(VERSION) && \
	docker push openthread/wpantund_amd64_linux:$(VERSION)

simulation: ot_simulation
	docker build -t openthread/simulation ot_sim && \
	docker push openthread/simulation && \
	docker tag openthread/simulation openthread/simulation:$(VERSION) && \
	docker push openthread/simulation:$(VERSION)

publish_manifest:
	docker manifest create openthread/wpantund openthread/wpantund_arm32v7_linux openthread/wpantund_amd64_linux
	docker manifest annotate openthread/wpantund openthread/wpantund_arm32v7_linux --os linux --arch arm --variant 7
	docker manifest annotate openthread/wpantund openthread/wpantund_amd64_linux --os linux --arch amd64
	docker manifest push --purge openthread/wpantund
	docker manifest create openthread/wpantund:$(VERSION) openthread/wpantund_arm32v7_linux:$(VERSION) openthread/wpantund_amd64_linux:$(VERSION)
	docker manifest annotate openthread/wpantund:$(VERSION) openthread/wpantund_arm32v7_linux:$(VERSION) --os linux --arch arm --variant 7
	docker manifest annotate openthread/wpantund:$(VERSION) openthread/wpantund_amd64_linux:$(VERSION) --os linux --arch amd64
	docker manifest push --purge openthread/wpantund:$(VERSION)
