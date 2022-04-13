
#include "HaasVideoCodecHal.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasVideoCodecHal::HaasVideoCodecHal()
{
    LOG_D("entern\n");
}

HaasVideoCodecHal::~HaasVideoCodecHal()
{
    LOG_D("entern\n");
}

int HaasVideoCodecHal::Open()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::DecodeConfig(DecodeConfig_t* config)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::StartDecode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::StopDecode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::GetDecodeImageData(void *pkt, ImageBuffer_t** image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::EncodeConfig(EncodeConfig_t* config)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::StartEncode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::StopEncode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecHal::GetEncodePacketData(VideoPacket_t **data)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

