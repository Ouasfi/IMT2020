#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/tree.hpp>
#include <ql/qldefines.hpp>
#ifdef BOOST_MSVC
#  include <ql/auto_link.hpp>
#endif
#include <ql/instruments/vanillaoption.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanengine.hpp>
#include <boost/timer.hpp>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <ctime>
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>


using namespace QuantLib;


int main() {



    try {

        // Date
        Calendar calendar = TARGET();
        Date todaysDate(8, Mar, 2020);
        Date settlementDate(10, Mar, 2020);
        Settings::instance().evaluationDate() = todaysDate;

        boost::timer timer;

        // Option
        Option::Type type(Option::Call);
        Real underlying = 100;
        Real strike = 100;
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.001;
        Volatility volatility = 0.20;
        Date maturity(10, Mar, 2021);
        DayCounter dayCounter = Actual365Fixed();

        std::cout << "Option type = "  << type << std::endl;
        std::cout << "Maturity = "   << maturity << std::endl;
        std::cout << "Underlying = "  << underlying << std::endl;
        std::cout << "Strike = "     << strike << std::endl;
        std::cout << "Risk free interest rate = " << io::rate(riskFreeRate) << std::endl;
        std::cout << "Dividend yield = " << io::rate(dividendYield) << std::endl;
        std::cout << "Volatility = " << io::volatility(volatility) << std::endl;
        std::cout << std::endl;
        std::string method;


        boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));
        Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));

        Handle<YieldTermStructure> flatTermStructure(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));
        Handle<BlackVolTermStructure> flatVolTS(boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, calendar, volatility,dayCounter)));
        Handle<YieldTermStructure> flatDividendTS(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));

    
        // Pay off
        boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));

        boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(new BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));

        // Type Option 
        VanillaOption europeanOption(payoff, europeanExercise);


        // B&S formula for Greeks

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess)));
        std::cout <<  std::endl;

        auto starttimeD = std::chrono::system_clock::now();

        std::cout << "Black & Scholes: "<<std::endl;
        std::cout << std::endl ;

        std::cout << "Delta Black & Scholes: "<<europeanOption.delta() << std::endl;

        auto endtimeD = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsD = endtimeD-starttimeD;
        std::time_t end_timeD = std::chrono::system_clock::to_time_t(endtimeD);
        std::cout << "Black & Scholes Delta time: " << elapsed_secondsD.count() << "s\n";

        auto starttimeG = std::chrono::system_clock::now();

        std::cout << "Gamma Black & Scholes: "<<europeanOption.gamma() << std::endl;

        auto endtimeG = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsG = endtimeG-starttimeG;
        std::time_t end_timeG = std::chrono::system_clock::to_time_t(endtimeG);
        std::cout << "Black & Scholes Gamma time: " << elapsed_secondsG.count() << "s\n";
        std::cout << std::endl ;
        std::cout << std::endl ;

        Size timeSteps = 801;

    

        // Cox Ross Rubistein
        std::cout << "Cox Ross Rubinstein: "<<std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<CoxRossRubinstein_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaCox = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaCox = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaCox = endtimeDeltaCox-starttimeDeltaCox;
        std::time_t end_timeDeltaCox = std::chrono::system_clock::to_time_t(endtimeDeltaCox);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaCox.count() << "s\n";

        auto starttimeGammaCox = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaCox = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaCox = endtimeGammaCox-starttimeGammaCox;
        std::time_t end_timeGammaCox = std::chrono::system_clock::to_time_t(endtimeGammaCox);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaCox.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess,timeSteps)));
        auto starttimeTreeDeltaCOld = std::chrono::system_clock::now();

        std::cout << "Delta Old: "<<europeanOption.delta() << std::endl;

        auto endtimeTreeDeltaCOld = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsTreeDeltaCOld = endtimeTreeDeltaCOld-starttimeTreeDeltaCOld;
        std::time_t end_timeTreeDeltaCOld = std::chrono::system_clock::to_time_t(endtimeTreeDeltaCOld);
        std::cout << "Binomial Tree Delta Old time: " << elapsed_secondsTreeDeltaCOld.count() << "s\n";

        auto starttimeTreeGammaCOld = std::chrono::system_clock::now();

        std::cout << "Gamma Old: "<<europeanOption.gamma() << std::endl;

        auto endtimeTreeGammaCOld = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsTreeGammaCOld = endtimeTreeGammaCOld-starttimeTreeGammaCOld;
        std::time_t end_timeTreeGammaCOld = std::chrono::system_clock::to_time_t(endtimeTreeGammaCOld);
        std::cout << "Binomial Tree Gamma Old time: " << elapsed_secondsTreeGammaCOld.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;
 

 

    
        // Trigeorgis
        std::cout << "Trigeorgis: " << std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Trigeorgis_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaT = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaT = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaT = endtimeDeltaT-starttimeDeltaT;
        std::time_t end_timeDeltaT = std::chrono::system_clock::to_time_t(endtimeDeltaT);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaT.count() << "s\n";

        auto starttimeGammaT = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaT = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaT = endtimeGammaT-starttimeGammaT;
        std::time_t end_timeGammaT = std::chrono::system_clock::to_time_t(endtimeGammaT);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaT.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Trigeorgis_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaTOld = std::chrono::system_clock::now();

        std::cout << "Delta Old: "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaTOld = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaTOld = endtimeDeltaTOld-starttimeDeltaTOld;
        std::time_t end_timeDeltaTOld = std::chrono::system_clock::to_time_t(endtimeDeltaTOld);
        std::cout << "Binomial tree Delta Old time: " << elapsed_secondsDeltaTOld.count() << "s\n";

        auto starttimeGammaTOld = std::chrono::system_clock::now();

        std::cout << "Gamma Old: "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaTOld = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaTOld = endtimeGammaTOld-starttimeGammaTOld;
        std::time_t end_timeGammaTOld = std::chrono::system_clock::to_time_t(endtimeGammaTOld);
        std::cout << "Binomial tree Gamma Old time: " << elapsed_secondsGammaTOld.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;





        // Jarrow Rudd
        std::cout << "Jarrow Rudd: " << std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaJ = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaJ = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaJ = endtimeDeltaJ-starttimeDeltaJ;
        std::time_t end_timeDeltaJ = std::chrono::system_clock::to_time_t(endtimeDeltaJ);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaJ.count() << "s\n";

        auto starttimeGammaJ = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaJ = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaJ = endtimeGammaJ-starttimeGammaJ;
        std::time_t end_timeGammaJ = std::chrono::system_clock::to_time_t(endtimeGammaJ);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaJ.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;


        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaJO = std::chrono::system_clock::now();

        std::cout << "Delta Old : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaJO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaJO = endtimeDeltaJO-starttimeDeltaJO;
        std::time_t end_timeDeltaJO = std::chrono::system_clock::to_time_t(endtimeDeltaJO);
        std::cout << "Binomial tree Delta Old time: " << elapsed_secondsDeltaJO.count() << "s\n";

        auto starttimeGammaJO = std::chrono::system_clock::now();

        std::cout << "Gamma Old : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaJO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaJO = endtimeGammaJO-starttimeGammaJO;
        std::time_t end_timeGammaJO = std::chrono::system_clock::to_time_t(endtimeGammaJO);
        std::cout << "Binomial tree Gamma Old time: " << elapsed_secondsGammaJO.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;





        // AdditiveEQPBinomialTree
        std::cout << "Additive EQP Binomial Tree: " << std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<AdditiveEQPBinomialTree_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaA = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaA = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaA = endtimeDeltaA-starttimeDeltaA;
        std::time_t end_timeDeltaA = std::chrono::system_clock::to_time_t(endtimeDeltaA);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaA.count() << "s\n";

        auto starttimeGammaA = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaA = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaA = endtimeGammaA-starttimeGammaA;
        std::time_t end_timeGammaA = std::chrono::system_clock::to_time_t(endtimeGammaA);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaA.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<AdditiveEQPBinomialTree_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaAO = std::chrono::system_clock::now();

        std::cout << "Delta Old : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaAO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaAO = endtimeDeltaA-starttimeDeltaAO;
        std::time_t end_timeDeltaAO = std::chrono::system_clock::to_time_t(endtimeDeltaAO);
        std::cout << "Binomial tree Delta Old time: " << elapsed_secondsDeltaAO.count() << "s\n";

        auto starttimeGammaAO = std::chrono::system_clock::now();

        std::cout << "Gamma Old : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaAO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaAO = endtimeGammaAO-starttimeGammaAO;
        std::time_t end_timeGammaAO = std::chrono::system_clock::to_time_t(endtimeGammaAO);
        std::cout << "Binomial tree Gamma Old time: " << elapsed_secondsGammaAO.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;

 




        // Tian
        std::cout << "Tian: " << std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Tian_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaTi = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaTi = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaTi = endtimeDeltaTi-starttimeDeltaTi;
        std::time_t end_timeDeltaTi = std::chrono::system_clock::to_time_t(endtimeDeltaTi);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaTi.count() << "s\n";

        auto starttimeGammaTi = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaTi = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaTi = endtimeGammaTi-starttimeGammaTi;
        std::time_t end_timeGammaTi = std::chrono::system_clock::to_time_t(endtimeGammaTi);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaTi.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Tian_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaTiO = std::chrono::system_clock::now();

        std::cout << "Delta Old : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaTiO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaTiO = endtimeDeltaTiO-starttimeDeltaTiO;
        std::time_t end_timeDeltaTiO = std::chrono::system_clock::to_time_t(endtimeDeltaTiO);
        std::cout << "Binomial tree Delta Old time: " << elapsed_secondsDeltaTiO.count() << "s\n";

        auto starttimeGammaTiO = std::chrono::system_clock::now();

        std::cout << "Gamma Old : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaTiO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaTiO = endtimeGammaTiO-starttimeGammaTiO;
        std::time_t end_timeGammaTiO = std::chrono::system_clock::to_time_t(endtimeGammaTiO);
        std::cout << "Binomial tree Gamma Old time: " << elapsed_secondsGammaTiO.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;






        // Leisen Reimer
        std::cout << "Leisen Reimer: " << std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<LeisenReimer_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaL = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaL = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaL = endtimeDeltaL-starttimeDeltaL;
        std::time_t end_timeDeltaL = std::chrono::system_clock::to_time_t(endtimeDeltaL);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaL.count() << "s\n";

        auto starttimeGammaL = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaL = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaL = endtimeGammaL-starttimeGammaL;
        std::time_t end_timeGammaL = std::chrono::system_clock::to_time_t(endtimeGammaL);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaL.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<LeisenReimer_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaLO = std::chrono::system_clock::now();

        std::cout << "Delta Old : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaLO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaLO = endtimeDeltaLO-starttimeDeltaLO;
        std::time_t end_timeDeltaLO = std::chrono::system_clock::to_time_t(endtimeDeltaLO);
        std::cout << "Binomial tree Delta Old time: " << elapsed_secondsDeltaLO.count() << "s\n";

        auto starttimeGammaLO = std::chrono::system_clock::now();

        std::cout << "Gamma Old: "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaLO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaLO = endtimeGammaLO-starttimeGammaLO;
        std::time_t end_timeGammaLO = std::chrono::system_clock::to_time_t(endtimeGammaLO);
        std::cout << "Binomial tree Gamma Old time: " << elapsed_secondsGammaLO.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;
        std::cout << std::endl ;






        // Joshi
        std::cout << "Joshi: " << std::endl;
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Joshi4_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaJo = std::chrono::system_clock::now();

        std::cout << "Delta : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaJo = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaJo = endtimeDeltaJo-starttimeDeltaJo;
        std::time_t end_timeDeltaJo = std::chrono::system_clock::to_time_t(endtimeDeltaJo);
        std::cout << "Binomial tree Delta time: " << elapsed_secondsDeltaJo.count() << "s\n";

        auto starttimeGammaJo = std::chrono::system_clock::now();

        std::cout << "Gamma : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaJo = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaJo = endtimeGammaJo-starttimeGammaJo;
        std::time_t end_timeGammaJo = std::chrono::system_clock::to_time_t(endtimeGammaJo);
        std::cout << "Binomial tree Gamma time: " << elapsed_secondsGammaJo.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;

        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine_2<Joshi4_2>(bsmProcess,timeSteps)));

        auto starttimeDeltaJoO = std::chrono::system_clock::now();

        std::cout << "Delta Old : "<<europeanOption.delta() << std::endl;

        auto endtimeDeltaJoO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsDeltaJoO = endtimeDeltaJoO-starttimeDeltaJoO;
        std::time_t end_timeDeltaJoO = std::chrono::system_clock::to_time_t(endtimeDeltaJoO);
        std::cout << "Binomial tree Delta Old time: " << elapsed_secondsDeltaJoO.count() << "s\n";

        auto starttimeGammaJoO = std::chrono::system_clock::now();

        std::cout << "Gamma Old : "<<europeanOption.gamma() << std::endl;

        auto endtimeGammaJoO = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_secondsGammaJoO = endtimeGammaJoO-starttimeGammaJoO;
        std::time_t end_timeGammaJoO = std::chrono::system_clock::to_time_t(endtimeGammaJoO);
        std::cout << "Binomial tree Gamma Old time: " << elapsed_secondsGammaJoO.count() << "s\n";
        std::cout << "Option Price: " << europeanOption.NPV() << "\n";
        std::cout << std::endl ;

        
    
        
        
        
        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
   
}

