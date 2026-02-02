#ifndef CROSS_SECTION
#define CROSS_SECTION
// ==================
// Cross_Srction: pb
// ==================
class GlobalConstants
{
public:
    // Unit pb^-1
    static constexpr float Lumi2017 = 41.5;
    // For DY Background
    static constexpr float HT0to70CS = 5706.3941; // 4274.424946; //need to be calculated for 2017 analysis
    static constexpr float HT70to100CS = 146.5;
    static constexpr float HT100to200CS = 160.7;
    static constexpr float HT200to400CS = 48.63;
    static constexpr float HT400to600CS = 6.993;
    static constexpr float HT600to800CS = 1.761;
    static constexpr float HT800to1200CS = 0.8021;
    static constexpr float HT1200to2500CS = 0.1937;
    static constexpr float HT2500toInfCS = 0.003514;
    //static constexpr float PT50CS = 344.3;
    //static constexpr float PT100CS = 80.64;
    //static constexpr float PT250CS = 2.955;
    //static constexpr float PT400CS = 0.3807;
    //static constexpr float PT650CS = 0.03711;

    // For Top Background
    static constexpr float ST_tW_top_5f_CS = 34.91;
    static constexpr float ST_tW_antitop_5f_CS = 34.97;
    static constexpr float TTTo2L2Nu_CS = 88.29;
    static constexpr float TTWJetsToLNu_CS = 0.2149;
    static constexpr float TTWJetsToQQ_CS = 0.4316;
    static constexpr float TTZToLLNuNu_CS = 0.2432;
    static constexpr float TTZToQQ_CS = 0.5104;
    // For Diboson Background
    //static constexpr float gg_WW_2L2Nu_CS = 0.5905;
    static constexpr float gg_ZZ_2e2mu_CS = 0.003291;
    static constexpr float gg_ZZ_2e2nu_CS = 0.001772;
    static constexpr float gg_ZZ_2e2tau_CS = 0.00329;
    static constexpr float gg_ZZ_2mu2nu_CS = 0.001772;
    static constexpr float gg_ZZ_2mu2tau_CS = 0.003289;
    static constexpr float gg_ZZ_4e_CS = 0.001405;
    static constexpr float gg_ZZ_4mu_CS = 0.001402;
    static constexpr float gg_ZZ_4tau_CS = 0.001407;
    static constexpr float qq_WW_2L2Nu_CS = 12.178;
    //static constexpr float qq_WZ_2L2Q_CS = 5.595 * 1.109;
    static constexpr float WZ_3LNu_CS = 5.052;
    static constexpr float ZZ_2L2Nu_CS = 0.5644;
    //static constexpr float qq_ZZ_2L2QCS = 3.220;
    static constexpr float ZZ_4L_CS = 1.256;

    // For Triboson Background
    static constexpr float WWZ_CS = 0.16510;
    static constexpr float WZZ_CS = 0.05565;
    static constexpr float ZZZ_CS = 0.01398;
};
#endif
