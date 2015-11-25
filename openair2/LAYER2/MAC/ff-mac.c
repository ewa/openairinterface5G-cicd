#include "ff-mac.h"
#include "ff-mac-sched-sap.h"
#include "ff-mac-csched-sap.h"
#include "ff-mac-init.h"
#include "log.h"
#include "assertions.h"

#undef LOG_D
#define LOG_D LOG_I

#include <stdlib.h>
#include <pthread.h>

/* number of callbacks */
#define N 9

/* this structure stores required data for OAI to work with FAPI */
/* it is the private version of fapi_interface_t */
struct fapi {
  fapi_interface_t fi;   /* the start of the structure matches fapi_interface_t */
  pthread_mutex_t mutex[N];
  pthread_cond_t cond[N];
  volatile int req_id[N];
  volatile int rsp_id[N];
  struct CschedCellConfigCnfParameters CschedCellConfigCnfParameters;
};

#define LOCK(fi, fn) do { \
    LOG_D(MAC, "%s: locking fn %d fi %p mutex %p\n", __FUNCTION__, fn, fi, &fi->mutex[fn]); \
    if (pthread_mutex_lock(&fi->mutex[fn])) \
      AssertFatal(0, "%s:%d:%s: mutex error\n", __FILE__, __LINE__, __FUNCTION__); \
  } while (0)

#define UNLOCK(fi, fn) do { \
    LOG_D(MAC, "%s: unlocking fn %d fi %p mutex %p\n", __FUNCTION__, fn, fi, &fi->mutex[fn]); \
    if (pthread_mutex_unlock(&fi->mutex[fn])) \
      AssertFatal(0, "%s:%d:%s: mutex error\n", __FILE__, __LINE__, __FUNCTION__); \
  } while (0)

#define CHECK(fi, fn) do { \
    if (fi->req_id[fn] != fi->rsp_id[fn]) \
      AssertFatal(0, "%s:%d:%s: check error\n", __FILE__, __LINE__, __FUNCTION__); \
  } while (0)

#define WAIT(fi, fn) do { \
    LOG_D(MAC, "%s: WAIT fn %d req %d rsp %d\n", __FUNCTION__, fn, fi->req_id[fn], fi->rsp_id[fn]); \
    while (fi->req_id[fn] == fi->rsp_id[fn]) \
      if (pthread_cond_wait(&fi->cond[fn], &fi->mutex[fn])) \
        AssertFatal(0, "%s:%d:%s: cond error\n", __FILE__, __LINE__, __FUNCTION__); \
  } while (0)

#define DONE_callback(fi, fn) do { \
    fi->req_id[fn]++; \
    if (pthread_cond_signal(&fi->cond[fn])) \
      AssertFatal(0, "%s:%d:%s: mutex error\n", __FILE__, __LINE__, __FUNCTION__); \
  } while (0)

#define DONE_wrapper(fi, fn) do { \
    fi->rsp_id[fn]++; \
  } while (0)

/* SCHED "wrappers" */

void SchedDlConfigInd(fapi_interface_t *_fi, struct SchedDlConfigIndParameters *params)
{
  int fn = 0;
}

void SchedUlConfigInd(fapi_interface_t *_fi, struct SchedUlConfigIndParameters *params)
{
  int fn = 1;
}

/* CSCHED "wrappers" */

void CschedCellConfigCnf(fapi_interface_t *_fi, struct CschedCellConfigCnfParameters *params)
{
  struct fapi *fi = (struct fapi *)_fi;
  int fn = 2;
  LOG_D(MAC, "CschedCellConfigCnf enter\n");

  LOCK(fi, fn);
  WAIT(fi, fn);

  *params = fi->CschedCellConfigCnfParameters;

  DONE_wrapper(fi, fn);
  UNLOCK(fi, fn);

  LOG_D(MAC, "CschedCellConfigCnf leave\n");
}

void CschedUeConfigCnf(fapi_interface_t *_fi, struct CschedUeConfigCnfParameters *params)
{
  int fn = 3;
}

void CschedLcConfigCnf(fapi_interface_t *_fi, struct CschedLcConfigCnfParameters *params)
{
  int fn = 4;
}

void CschedLcReleaseCnf(fapi_interface_t *_fi, struct CschedLcReleaseCnfParameters *params)
{
  int fn = 5;
}

void CschedUeReleaseCnf(fapi_interface_t *_fi, struct CschedUeReleaseCnfParameters *params)
{
  int fn = 6;
}

void CschedUeConfigUpdateInd(fapi_interface_t *_fi, struct CschedUeConfigUpdateIndParameters *params)
{
  int fn = 7;
}

void CschedCellConfigUpdateInd(fapi_interface_t *_fi, struct CschedCellConfigUpdateIndParameters *params)
{
  int fn = 8;
}

/* SCHED callbacks */

void SchedDlConfigInd_callback(void *callback_data, const struct SchedDlConfigIndParameters *params)
{
  int fn = 0;
}

void SchedUlConfigInd_callback(void *callback_data, const struct SchedUlConfigIndParameters *params)
{
  int fn = 1;
}

/* CSCHED callbacks */

void CschedCellConfigCnf_callback(void *callback_data, const struct CschedCellConfigCnfParameters *params)
{
  struct fapi *fi = callback_data;
  int fn = 2;
  LOG_D(MAC, "CschedCellConfigCnf_callback enter\n");

  LOCK(fi, fn);
  CHECK(fi, fn);

  fi->CschedCellConfigCnfParameters = *params;

  DONE_callback(fi, fn);
  UNLOCK(fi, fn);

  LOG_D(MAC, "CschedCellConfigCnf_callback leave\n");
}

void CschedUeConfigCnf_callback(void *callback_data, const struct CschedUeConfigCnfParameters *params)
{
  int fn = 3;
}

void CschedLcConfigCnf_callback(void *callback_data, const struct CschedLcConfigCnfParameters *params)
{
  int fn = 4;
}

void CschedLcReleaseCnf_callback(void *callback_data, const struct CschedLcReleaseCnfParameters *params)
{
  int fn = 5;
}

void CschedUeReleaseCnf_callback(void *callback_data, const struct CschedUeReleaseCnfParameters *params)
{
  int fn = 6;
}

void CschedUeConfigUpdateInd_callback(void *callback_data, const struct CschedUeConfigUpdateIndParameters *params)
{
  int fn = 7;
}

void CschedCellConfigUpdateInd_callback(void *callback_data, const struct CschedCellConfigUpdateIndParameters *params)
{
  int fn = 8;
}

fapi_interface_t *init_fapi(void)
{
  struct fapi *ret;
  int i;

  LOG_I(MAC, "FAPI initialization\n");

  ret = calloc(1, sizeof(struct fapi));
  if (ret == NULL) {
    LOG_E(MAC, "init_fapi: memory allocation error\n");
    return NULL;
  }

  for (i = 0; i < N; i++) {
    if (pthread_mutex_init(&ret->mutex[i], NULL)) {
      LOG_E(MAC, "init_fapi: mutex init error\n");
      exit(1);
    }
    if (pthread_cond_init(&ret->cond[i], NULL)) {
      LOG_E(MAC, "init_fapi: cond init error\n");
      exit(1);
    }
    ret->req_id[i] = 0;
    ret->rsp_id[i] = 0;
  }

  ret->fi.sched = SchedInit(ret,
                      SchedDlConfigInd_callback,
                      SchedUlConfigInd_callback,
                      CschedCellConfigCnf_callback,
                      CschedUeConfigCnf_callback,
                      CschedLcConfigCnf_callback,
                      CschedLcReleaseCnf_callback,
                      CschedUeReleaseCnf_callback,
                      CschedUeConfigUpdateInd_callback,
                      CschedCellConfigUpdateInd_callback);

  if (ret->fi.sched == NULL) {
    LOG_E(MAC, "init_fapi: SchedInit failed\n");
    free(ret);
    return NULL;
  }

  return (fapi_interface_t *)ret;
}
