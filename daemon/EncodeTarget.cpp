#include "EncodeTarget.h"

EncodeTarget::EncodeTarget(const QStringList &handBrakeArguments, const QString &suffix, const QString &name) :
	m_handBrakeArguments(handBrakeArguments),
	m_suffix(suffix),
	m_name(name)
{
}
QStringList EncodeTarget::handBrakeArguments() const
{
	return m_handBrakeArguments;
}
QString EncodeTarget::suffix() const
{
	return m_suffix;
}
QString EncodeTarget::name() const
{
	return m_name;
}
QList<EncodeTarget> EncodeTarget::targets()
{
	static QList<EncodeTarget> targets;
	if (targets.length() != 0)
		return targets;
	targets.append(EncodeTarget(
			QStringList()
				<< "-f" << "mp4"
				<< "-w" << "640"
				<< "--decomb"
				<< "-e" << "x264"
				<< "-b" << "500"
				<< "-2"
				<< "-a" << "1"
				<< "-r" << "25"
				<< "-E" << "faac"
				<< "-6" << "dpl2"
				<< "-R" << "48"
				<< "-B" << "96"
				<< "-x" << "ref=2:bframes=2:subq=6:mixed-refs=0:weightb=0:8x8dct=0:trellis=0",
			"-streaming.mp4",
			"Streaming"
	));
	targets.append(EncodeTarget(
			QStringList()
				<< "-f" << "mp4"
				<< "-O"
				<< "-w" << "720"
				<< "--loose-anamorphic"
				<< "--detelecine"
				<< "--decomb"
				<< "-e" << "x264"
				<< "-b" << "800"
				<< "-a" << "1,1"
				<< "-2"
				<< "-r" << "25"
				<< "-E" << "faac,ac3"
				<< "-6" << "dpl2,auto"
				<< "-R" << "48,Auto"
				<< "-B" << "128,auto"
				<< "-x" << "b-adapt=2:rc-lookahead=50:keyint=50:min-keyint=25",
			"-pd-high.mp4",
			"PD High"
	));
	targets.append(EncodeTarget(
			QStringList()
				<< "-f" << "mp4"
				<< "-O"
				<< "-X" << "480"
				<< "-e" << "x264"
				<< "-b" << "350"
				<< "-2"
				<< "-r" << "25"
				<< "-a" << "1"
				<< "-E" << "faac"
				<< "-6" << "dpl2"
				<< "-R" << "48"
				<< "-B" << "128"
				<< "-x" << "cabac=0:ref=2:me=umh:bframes=0:subq=6:8x8dct=0:trellis=0:weightb=0:keyint=50:min-keyint=25",
			"-pd-low.mp4",
			"PD Low"
	));
	return targets;
}
