copy /Y projects\%1\ql_target.cmake components\ql-application\
copy /Y projects\%1\ql_app_feature_config.h.in components\ql-application\
copy /Y projects\%1\clockface_table.c components\ql-application\lv_widgets\clockface\
copy /Y projects\%1\digital1 components\ql-application\lv_widgets\assets\images\clockface\digital1\
copy /Y projects\%1\digital2 components\ql-application\lv_widgets\assets\images\clockface\digital2\
copy /Y projects\%1\digital3 components\ql-application\lv_widgets\assets\images\clockface\digital3\
call resgen
call build_all new EC600UCN_LB C51 VOLTE SINGLESIM release


