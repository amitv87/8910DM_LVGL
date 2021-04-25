copy /Y projects\%1\ql_target.cmake components\ql-application\
copy /Y projects\%1\ql_app_feature_config.h.in components\ql-application\
call resgen
call build_all new EC600UCN_LB C51 VOLTE SSIM debug