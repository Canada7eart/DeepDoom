#include "stdafx.h"
#include "FrameFingerprint.h"

FrameFingerprint operator-(const FrameFingerprint& one, const FrameFingerprint& two)
{
	return FrameFingerprint(one.fingerprint - two.fingerprint);
}
FrameFingerprint operator+(const FrameFingerprint& one, const FrameFingerprint& two)
{
	return FrameFingerprint(one.fingerprint + two.fingerprint);
}
FrameFingerprint operator-(const FrameFingerprint& one, const double& two)
{
	return FrameFingerprint(one.fingerprint - two);
}
FrameFingerprint operator+(const FrameFingerprint& one, const double& two)
{
	return FrameFingerprint(one.fingerprint + two);
}