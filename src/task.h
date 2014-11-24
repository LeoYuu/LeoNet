#ifndef __TASK_H__
#define __TASK_H__

class task
{
public:
  virtual void doing() = 0;
};

class logictask : public task
{
public:
  logictask();
  ~logictask();

public:
  virtual void doing();

private:

};

#endif /* __TASK_H__ */
