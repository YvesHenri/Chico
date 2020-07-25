#pragma once

#include <Engine/Messages/Message.hpp>

struct CameraChanged : public mqs::ManagedMessage<CameraChanged>
{
	CameraChanged() : ManagedMessage(1U) {}
};