#ifndef RIPTASK_H
#define RIPTASK_H

#include "Task.h"
#include <QReadWriteLock>
class QIODevice;

class RipTask : public Task
{
	Q_OBJECT
public:
	RipTask(QObject *parent = 0);
	~RipTask();
	bool saveImageToDevice(QIODevice &out);
	bool saveImageToPath(const QString &path);
	bool canRunTask(const Movie *movie) const;
	QString status() const;

public slots:
	void kill();

private:
	static int cmpvob(const void *p1, const void *p2);
	typedef struct vobfile {
		int32_t start, end;
	} vobfile;
	QReadWriteLock m_locker;
	bool m_terminate;
	QString m_status;

protected:
	bool executeTask(Movie *movie);
	void cleanUp(bool result);

signals:
	void extractProgress(int current, int total);
};

#endif // RIPTASK_H
