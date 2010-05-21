#ifndef UPLOADTASK_H
#define UPLOADTASK_H

#include "Task.h"

class UploadTask : public Task
{
	Q_OBJECT
public:
	UploadTask(QObject *parent = 0);
	~UploadTask();
	bool canRunTask(const Movie &movie) const;

protected:
	bool executeTask(Movie &movie);
	void kill();

};

#endif // UPLOADTASK_H
