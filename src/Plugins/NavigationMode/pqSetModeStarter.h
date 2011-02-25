#ifndef __pqSetModeStarter_h 
#define __pqSetModeStarter_h

#include <QObject>

class pqSetModeStarter : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;
public:
  pqSetModeStarter(QObject* p=0);
  ~pqSetModeStarter();

  // Callback for startup.
  void onStartup();

  // Callback for shutdown.
  void shutdown() {}

private:
  void setStandardMode();

private:
  pqSetModeStarter(const pqSetModeStarter&); // Not implemented.
  void operator=(const pqSetModeStarter&); // Not implemented.
};

#endif


