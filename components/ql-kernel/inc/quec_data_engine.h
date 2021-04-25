#ifndef QUEC_DATA_ENGINE_H
#define QUEC_DATA_ENGINE_H

#define QL_DATA_ENGINE_RX_ARRIVED      101
#define QL_DATA_ENGINE_TX_ARRIVED      102
#define QL_DATA_ENGINE_TX_COMPLETE     103

typedef void (*ql_data_engine_event_cb)(atDispatch_t  *ch, int event);

int quec_data_engine_register(atDispatch_t *ch, ql_data_engine_event_cb  event_cb);

int quec_data_engine_unregister(atDispatch_t *ch);

int quec_data_engine_notify(atDispatch_t  *ch, int event);

int quec_data_engine_read(atDispatch_t *ch, void *data, size_t size);

int quec_data_engine_write(atDispatch_t *ch, void *data, size_t size);

int quec_data_engine_read_avai(atDispatch_t *ch);

int quec_data_engine_write_avai(atDispatch_t *ch);

#endif
