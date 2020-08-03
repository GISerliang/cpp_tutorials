#ifndef NONCOPYABLE_H_INCLUDED
#define NONCOPYABLE_H_INCLUDED

class noncopyable {
 private:

 protected:
  noncopyable() = default;
  noncopyable ( const noncopyable& ) = delete;
  noncopyable& operator= ( const noncopyable& ) = delete;
};

#endif // NONCOPYABLE_H_INCLUDED
