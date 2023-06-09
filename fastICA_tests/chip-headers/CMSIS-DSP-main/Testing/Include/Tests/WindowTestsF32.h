#include "Test.h"
#include "Pattern.h"

#include "dsp/window_functions.h"

class WindowTestsF32:public Client::Suite
    {
        public:
            WindowTestsF32(Testing::testID_t id);
            virtual void setUp(Testing::testID_t,std::vector<Testing::param_t>& params,Client::PatternMgr *mgr);
            virtual void tearDown(Testing::testID_t,Client::PatternMgr *mgr);
        private:
            #include "WindowTestsF32_decl.h"
            
            Client::LocalPattern<float32_t> output;
            // Reference patterns are not loaded when we are in dump mode
            Client::RefPattern<float32_t> ref;

    };
