/*  impl.c.locknt
 *
 *                  RECURSIVE LOCKS IN WIN32
 *
 *  $HopeName: !locknt.c(trunk.1) $
 *
 *  Copyright (C) 1995 Harlequin Group, all rights reserved
 *
 *  These are implemented using critical sections.
 *  See the section titled "Synchronization functions" in the Groups
 *  chapter of the Microsoft Win32 API Programmer's Reference.
 *  The "Synchronization" section of the Overview is also relevant.
 *
 *  Critical sections support recursive locking, so the implementation
 *  could be trivial.  This implementation counts the claims to provide
 *  extra checking.
 *
 *  The limit on the number of recursive claims is the max of
 *  ULONG_MAX and the limit imposed by critical sections, which
 *  is believed to be about UCHAR_MAX.
 *
 *  During use the claims field is updated to remember the number of
 *  claims acquired on a lock.  This field must only be modified
 *  while we are inside the critical section.
 */

#include "std.h"
#include "lock.h"
#include "lockst.h"

#ifndef OS_NT
#error "locknt.c is specific to Win32 but OS_NT not defined"
#endif

#include <windows.h>

#ifdef DEBUG_SIGN
static SigStruct LockSigStruct;
#endif

#ifdef DEBUG_ASSERT

Bool LockIsValid(Lock lock, ValidationType validParam)
{
#ifdef DEBUG_SIGN
  AVER(ISVALIDNESTED(Sig, &LockSigStruct));
  AVER(lock->sig == &LockSigStruct);
#endif
  return TRUE;
}  

#endif

void LockInit(Lock lock)
{
  AVER(lock != NULL);
  lock->claims = 0; 
  InitializeCriticalSection(&lock->cs);
#ifdef DEBUG_SIGN
  SigInit(&LockSigStruct, "Lock");
  lock->sig = &LockSigStruct;
#endif
  AVER(ISVALID(Lock, lock));
}

void LockFinish(Lock lock)
{
  AVER(ISVALID(Lock, lock));
  /* Lock should not be finished while held */
  AVER(lock->claims == 0);
  DeleteCriticalSection(&lock->cs);
#ifdef DEBUG_SIGN
  lock->sig = SigInvalid;
#endif
}

void LockClaim(Lock lock)
{
  AVER(ISVALID(Lock, lock));
  EnterCriticalSection(&lock->cs);
  /* This should be the first claim.  Now we are inside the
   * critical section it is ok to check this. */
  AVER(lock->claims == 0);
  lock->claims = 1;        
}

void LockRelease(Lock lock)
{
  AVER(ISVALID(Lock, lock));
  AVER(lock->claims == 1);  /* The lock should only be held once */
  lock->claims = 0;  /* Must set this before leaving CS */
  LeaveCriticalSection(&lock->cs);
}

void LockClaimRecursive(Lock lock)
{
  AVER(ISVALID(Lock, lock));
  EnterCriticalSection(&lock->cs);
  ++lock->claims;
  AVER(lock->claims > 0);
}

void LockReleaseRecursive(Lock lock)
{
  AVER(ISVALID(Lock, lock));
  AVER(lock->claims > 0);
  --lock->claims;
  LeaveCriticalSection(&lock->cs);
}
