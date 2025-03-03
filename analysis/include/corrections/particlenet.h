#ifndef PARTICLENET_H
#define PARTICLENET_H

// VBS
#include "corrections/sfs.h"       // NanoSFsUL, SFHist
// ROOT
#include "TString.h"

struct VBSWHXbbSFs : NanoSFsUL
{
private:
    double boohftCalib(std::string year, double pt, std::string var = "nominal")
    {
        if (year == "2018")
        {
            /* Markdown table from web/VBSWH_mkW_Mjj100toInf_Htobb_dipoleRecoilOn_bb_ULNanoV9_PNetXbbVsQCD_ak8_2018/4_fit/index.html:
               |       | pT [250, 500) | pT [500, 700) | pT [700, +inf) |
               | :---: | :---: | :---: | :---: |
               | **High Purity** WP | **1.000** [-0.024/+0.024] | **1.029** [-0.023/+0.034] | **1.026** [-0.016/+0.025] |
            */
            if (pt >= 250 && pt < 500)
            {
                if (var == "nominal") { return 1.000; }
                else if (var == "up") { return 1.000+0.024; }
                else if (var == "dn") { return 1.000-0.024; }
            }
            else if (pt >= 500 && pt < 700)
            {
                if (var == "nominal") { return 1.029; }
                else if (var == "up") { return 1.029+0.034; }
                else if (var == "dn") { return 1.029-0.023; }
            }
            else if (pt >= 700)
            {
                if (var == "nominal") { return 1.026; }
                else if (var == "up") { return 1.026+0.025; }
                else if (var == "dn") { return 1.026-0.016; }
            }
        }
        else if (year == "2017")
        {
            /* Markdown table from web/VBSWH_mkW_Mjj100toInf_Htobb_dipoleRecoilOn_bb_ULNanoV9_PNetXbbVsQCD_ak8_2017/4_fit/index.html:
               |       | pT [250, 500) | pT [500, 700) | pT [700, +inf) |
               | :---: | :---: | :---: | :---: |
               | **High Purity** WP | **1.021** [-0.024/+0.028] | **1.018** [-0.022/+0.031] | **1.010** [-0.018/+0.019] |
            */
            if (pt >= 250 && pt < 500)
            {
                if (var == "nominal") { return 1.021; }
                else if (var == "up") { return 1.021+0.028; }
                else if (var == "dn") { return 1.021-0.024; }
            }
            else if (pt >= 500 && pt < 700)
            {
                if (var == "nominal") { return 1.018; }
                else if (var == "up") { return 1.018+0.031; }
                else if (var == "dn") { return 1.018-0.022; }
            }
            else if (pt >= 700)
            {
                if (var == "nominal") { return 1.010; }
                else if (var == "up") { return 1.010+0.019; }
                else if (var == "dn") { return 1.010-0.018; }
            }
        }
        else if (year == "2016postVFP")
        {
            /* Markdown table from web/VBSWH_mkW_Mjj100toInf_Htobb_dipoleRecoilOn_bb_ULNanoV9_PNetXbbVsQCD_ak8_2016/4_fit/index.html:
               |       | pT [250, 500) | pT [500, 700) | pT [700, +inf) |
               | :---: | :---: | :---: | :---: |
               | **High Purity** WP | **1.044** [-0.040/+0.052] | **1.081** [-0.065/+0.080] | **0.996** [-0.039/+0.035] |
            */
            if (pt >= 250 && pt < 500)
            {
                if (var == "nominal") { return 1.044; }
                else if (var == "up") { return 1.044+0.052; }
                else if (var == "dn") { return 1.044-0.040; }
            }
            else if (pt >= 500 && pt < 700)
            {
                if (var == "nominal") { return 1.081; }
                else if (var == "up") { return 1.081+0.080; }
                else if (var == "dn") { return 1.081-0.065; }
            }
            else if (pt >= 700)
            {
                if (var == "nominal") { return 0.996; }
                else if (var == "up") { return 0.996+0.035; }
                else if (var == "dn") { return 0.996-0.039; }
            }
        }
        else if (year == "2016preVFP")
        {
            /* Markdown table from web/VBSWH_mkW_Mjj100toInf_Htobb_dipoleRecoilOn_bb_ULNanoV9_PNetXbbVsQCD_ak8_2016APV/4_fit/index.html:
               |       | pT [250, 500) | pT [500, 700) | pT [700, +inf) |
               | :---: | :---: | :---: | :---: |
               | **High Purity** WP | **1.008** [-0.063/+0.064] | **1.027** [-0.055/+0.070] | **1.014** [-0.055/+0.057] |
            */
            if (pt >= 250 && pt < 500)
            {
                if (var == "nominal") { return 1.008; }
                else if (var == "up") { return 1.008+0.064; }
                else if (var == "dn") { return 1.008-0.063; }
            }
            else if (pt >= 500 && pt < 700)
            {
                if (var == "nominal") { return 1.027; }
                else if (var == "up") { return 1.027+0.070; }
                else if (var == "dn") { return 1.027-0.055; }
            }
            else if (pt >= 700)
            {
                if (var == "nominal") { return 1.014; }
                else if (var == "up") { return 1.014+0.057; }
                else if (var == "dn") { return 1.014-0.055; }
            }
        }
        return 1.;
    };
public:
    std::string year_str;

    VBSWHXbbSFs() { /* Do nothing */ };

    void init(TString file_name)
    {
        NanoSFsUL::init(file_name);

        switch (campaign)
        {
        case (RunIISummer20UL16APV):
            year_str = "2016preVFP";
            break;
        case (RunIISummer20UL16):
            year_str = "2016postVFP";
            break;
        case (RunIISummer20UL17):
            year_str = "2017";
            break;
        case (RunIISummer20UL18):
            year_str = "2018";
            break;
        default:
            return;
            break;
        };
    };

    double getSF(double pt)
    {
        return boohftCalib(year_str, pt);
    };

    double getSFUp(double pt)
    {
        return boohftCalib(year_str, pt, "up");
    };

    double getSFDn(double pt)
    {
        return boohftCalib(year_str, pt, "dn");
    };
};
// https://lutton-vvh-sfbdt.web.cern.ch/H_bb_PNetXbbVsQCD-Eslam_NoEFR_2018/4_fit/
struct VBSVVHXbbSFs : NanoSFsUL
{
private:
    double boohftCalib(std::string year, double pt, std::string var = "nominal")
    {
        if (year == "2018")
        {
            /*
              2018 bb-tagging SF (SF_flvB), High Purity WP
              pT [300, 400)   pT [400, 500)   pT [500, 600)   pT [600, +∞)
              0.989 [-0.068/+0.064]   1.088 [-0.135/+0.139]   0.957 [-0.033/+0.031]   1.051 [-0.036/+0.041]
            */
            if (pt >= 300 && pt < 400)
            {
                if (var == "nominal") { return 0.989; }
                else if (var == "up") { return 0.989 + 0.064; }
                else if (var == "dn") { return 0.989 - 0.068; }
            }
            else if (pt >= 400 && pt < 500)
            {
                if (var == "nominal") { return 1.088; }
                else if (var == "up") { return 1.088 + 0.139; }
                else if (var == "dn") { return 1.088 - 0.135; }
            }
            else if (pt >= 500 && pt < 600)
            {
                if (var == "nominal") { return 0.957; }
                else if (var == "up") { return 0.957 + 0.031; }
                else if (var == "dn") { return 0.957 - 0.033; }
            }
            else if (pt >= 600)
            {
                if (var == "nominal") { return 1.051; }
                else if (var == "up") { return 1.051 + 0.041; }
                else if (var == "dn") { return 1.051 - 0.036; }
            }
        }
        if (year == "2017")
        {
            /*
              2017 bb-tagging SF (SF_flvB), High Purity WP
              pT [300, 400)   pT [400, 500)   pT [500, 600)   pT [600, +∞)
              0.992 [-0.146/+0.146]   1.090 [-0.096/+0.099]   1.082 [-0.059/+0.063]   1.075 [-0.077/+0.079]
            */
            if (pt >= 300 && pt < 400)
            {
                if (var == "nominal") { return 0.992; }
                else if (var == "up") { return 0.992 + 0.146; }
                else if (var == "dn") { return 0.992 - 0.146; }
            }
            else if (pt >= 400 && pt < 500)
            {
                if (var == "nominal") { return 1.090; }
                else if (var == "up") { return 1.090 + 0.099; }
                else if (var == "dn") { return 1.090 - 0.096; }
            }
            else if (pt >= 500 && pt < 600)
            {
                if (var == "nominal") { return 1.082; }
                else if (var == "up") { return 1.082 + 0.063; }
                else if (var == "dn") { return 1.082 - 0.059; }
            }
            else if (pt >= 600)
            {
                if (var == "nominal") { return 1.075; }
                else if (var == "up") { return 1.075 + 0.079; }
                else if (var == "dn") { return 1.075 - 0.077; }
            }
        }

        if (year == "2016postVFP")
        {
            /*
              2016 (postVFP) bb-tagging SF (SF_flvB), High Purity WP
              pT [300, 400)   pT [400, 500)   pT [500, 600)   pT [600, +∞)
              0.975 [-0.124/+0.122]   1.003 [-0.122/+0.124]   0.993 [-0.090/+0.104]   1.131 [-0.120/+0.240]
            */
            if (pt >= 300 && pt < 400)
            {
                if (var == "nominal") { return 0.975; }
                else if (var == "up") { return 0.975 + 0.122; }
                else if (var == "dn") { return 0.975 - 0.124; }
            }
            else if (pt >= 400 && pt < 500)
            {
                if (var == "nominal") { return 1.003; }
                else if (var == "up") { return 1.003 + 0.124; }
                else if (var == "dn") { return 1.003 - 0.122; }
            }
            else if (pt >= 500 && pt < 600)
            {
                if (var == "nominal") { return 0.993; }
                else if (var == "up") { return 0.993 + 0.104; }
                else if (var == "dn") { return 0.993 - 0.090; }
            }
            else if (pt >= 600)
            {
                if (var == "nominal") { return 1.131; }
                else if (var == "up") { return 1.131 + 0.240; }
                else if (var == "dn") { return 1.131 - 0.120; }
            }
        }

        if (year == "2016preVFP")
        {
            /*
              2016APV (preVFP) bb-tagging SF (SF_flvB), High Purity WP
              pT [300, 400)   pT [400, 500)   pT [500, 600)   pT [600, +∞)
              0.993 [-0.100/+0.113]   1.000 [-0.065/+0.106]   1.049 [-0.067/+0.132]   1.179 [-0.185/+0.339]
            */
            if (pt >= 300 && pt < 400)
            {
                if (var == "nominal") { return 0.993; }
                else if (var == "up") { return 0.993 + 0.113; }
                else if (var == "dn") { return 0.993 - 0.100; }
            }
            else if (pt >= 400 && pt < 500)
            {
                if (var == "nominal") { return 1.000; }
                else if (var == "up") { return 1.000 + 0.106; }
                else if (var == "dn") { return 1.000 - 0.065; }
            }
            else if (pt >= 500 && pt < 600)
            {
                if (var == "nominal") { return 1.049; }
                else if (var == "up") { return 1.049 + 0.132; }
                else if (var == "dn") { return 1.049 - 0.067; }
            }
            else if (pt >= 600)
            {
                if (var == "nominal") { return 1.179; }
                else if (var == "up") { return 1.179 + 0.339; }
                else if (var == "dn") { return 1.179 - 0.185; }
            }
        }

        // If none of the above conditions match (or year not recognized), return 1.
        return 1.;
    }
public:
    std::string year_str;

    VBSVVHXbbSFs() { /* Do nothing */ };

    void init(TString file_name)
    {
        NanoSFsUL::init(file_name);

        switch (campaign)
        {
        case (RunIISummer20UL16APV):
            year_str = "2016preVFP";
            break;
        case (RunIISummer20UL16):
            year_str = "2016postVFP";
            break;
        case (RunIISummer20UL17):
            year_str = "2017";
            break;
        case (RunIISummer20UL18):
            year_str = "2018";
            break;
        default:
            return;
            break;
        };
    };

    double getSF(double pt)
    {
        return boohftCalib(year_str, pt);
    };

    double getSFUp(double pt)
    {
        return boohftCalib(year_str, pt, "up");
    };

    double getSFDn(double pt)
    {
        return boohftCalib(year_str, pt, "dn");
    };
};
// struct VBSVVHXbbSFs : NanoSFsUL (old numbers for all merged from jonathan's analysis)
// {
// private:
//     double boohftCalib(std::string year, double pt, std::string var = "nominal")
//     {
//         if (year == "2018")
//         {
//             /* Markdown table from web/H_bb_PNetXbbVsQCD-JGuiang_ChangedCut_2018/4_fit/index.html:
//                |       | pT [300, 400) | pT [400, 500) | pT [500, 600) | pT [600, +∞) |
//                | :---: | :---: | :---: | :---: | :---: |
//                | **High Purity** WP | **0.875** [-0.077/+0.071] | **0.895** [-0.102/+0.100] | **1.030** [-0.078/+0.081] | **0.933** [-0.095/+0.093] |
//             */
//             if (pt >= 300 && pt < 400)
//             {
//                 if (var == "nominal") { return 0.875; }
//                 else if (var == "up") { return 0.875+0.071; }
//                 else if (var == "dn") { return 0.875-0.077; }
//             }
//             else if (pt >= 400 && pt < 500)
//             {
//                 if (var == "nominal") { return 0.895; }
//                 else if (var == "up") { return 0.895+0.100; }
//                 else if (var == "dn") { return 0.895-0.102; }
//             }
//             else if (pt >= 500 && pt < 600)
//             {
//                 if (var == "nominal") { return 1.030; }
//                 else if (var == "up") { return 1.030+0.081; }
//                 else if (var == "dn") { return 1.030-0.078; }
//             }
//             else if (pt >= 600)
//             {
//                 if (var == "nominal") { return 0.933; }
//                 else if (var == "up") { return 0.933+0.093; }
//                 else if (var == "dn") { return 0.933-0.095; }
//             }
//         }
//         if (year == "2017")
//         {
//             /* Markdown table from web/H_bb_PNetXbbVsQCD-JGuiang_ChangedCut_2017/4_fit/index.html:
//                |       | pT [300, 400) | pT [400, 500) | pT [500, 600) | pT [600, +∞) |
//                | :---: | :---: | :---: | :---: | :---: |
//                | **High Purity** WP | **0.996** [-0.225/+0.226] | **1.068** [-0.131/+0.138] | **1.012** [-0.202/+0.202] | **1.062** [-0.061/+0.070] |
//             */
//             if (pt >= 300 && pt < 400)
//             {
//                 if (var == "nominal") { return 0.996; }
//                 else if (var == "up") { return 0.996+0.226; }
//                 else if (var == "dn") { return 0.996-0.225; }
//             }
//             else if (pt >= 400 && pt < 500)
//             {
//                 if (var == "nominal") { return 1.068; }
//                 else if (var == "up") { return 1.068+0.138; }
//                 else if (var == "dn") { return 1.068-0.131; }
//             }
//             else if (pt >= 500 && pt < 600)
//             {
//                 if (var == "nominal") { return 1.012; }
//                 else if (var == "up") { return 1.012+0.202; }
//                 else if (var == "dn") { return 1.012-0.202; }
//             }
//             else if (pt >= 600)
//             {
//                 if (var == "nominal") { return 1.062; }
//                 else if (var == "up") { return 1.062+0.070; }
//                 else if (var == "dn") { return 1.062-0.061; }
//             }
//         }
//         if (year == "2016postVFP")
//         {
//             /* Markdown table from web/H_bb_PNetXbbVsQCD-JGuiang_ChangedCut_2016/4_fit/index.html:
//                |       | pT [300, 400) | pT [400, 500) | pT [500, 600) | pT [600, +∞) |
//                | :---: | :---: | :---: | :---: | :---: |
//                | **High Purity** WP | **1.027** [-0.096/+0.098] | **1.139** [-0.074/+0.093] | **1.147** [-0.124/+0.137] | **1.279** [-0.223/+0.250] |
//             */
//             if (pt >= 300 && pt < 400)
//             {
//                 if (var == "nominal") { return 1.027; }
//                 else if (var == "up") { return 1.027+0.098; }
//                 else if (var == "dn") { return 1.027-0.096; }
//             }
//             else if (pt >= 400 && pt < 500)
//             {
//                 if (var == "nominal") { return 1.139; }
//                 else if (var == "up") { return 1.139+0.093; }
//                 else if (var == "dn") { return 1.139-0.074; }
//             }
//             else if (pt >= 500 && pt < 600)
//             {
//                 if (var == "nominal") { return 1.147; }
//                 else if (var == "up") { return 1.147+0.137; }
//                 else if (var == "dn") { return 1.147-0.124; }
//             }
//             else if (pt >= 600)
//             {
//                 if (var == "nominal") { return 1.279; }
//                 else if (var == "up") { return 1.279+0.250; }
//                 else if (var == "dn") { return 1.279-0.223; }
//             }
//         }
//         if (year == "2016preVFP")
//         {
//             /* Markdown table from web/H_bb_PNetXbbVsQCD-JGuiang_ChangedCut_2016APV/4_fit/index.html:
//                |       | pT [300, 400) | pT [400, 500) | pT [500, 600) | pT [600, +∞) |
//                | :---: | :---: | :---: | :---: | :---: |
//                | **High Purity** WP | **1.163** [-0.107/+0.119] | **1.151** [-0.129/+0.137] | **1.210** [-0.211/+0.232] | **1.288** [-0.324/+0.345] |
//             */
//             if (pt >= 300 && pt < 400)
//             {
//                 if (var == "nominal") { return 1.163; }
//                 else if (var == "up") { return 1.163+0.119; }
//                 else if (var == "dn") { return 1.163-0.107; }
//             }
//             else if (pt >= 400 && pt < 500)
//             {
//                 if (var == "nominal") { return 1.151; }
//                 else if (var == "up") { return 1.151+0.137; }
//                 else if (var == "dn") { return 1.151-0.129; }
//             }
//             else if (pt >= 500 && pt < 600)
//             {
//                 if (var == "nominal") { return 1.210; }
//                 else if (var == "up") { return 1.210+0.232; }
//                 else if (var == "dn") { return 1.210-0.211; }
//             }
//             else if (pt >= 600)
//             {
//                 if (var == "nominal") { return 1.288; }
//                 else if (var == "up") { return 1.288+0.345; }
//                 else if (var == "dn") { return 1.288-0.324; }
//             }
//         }
//         return 1.;
//     }
// public:
//     std::string year_str;
//
//     VBSVVHXbbSFs() { /* Do nothing */ };
//
//     void init(TString file_name)
//     {
//         NanoSFsUL::init(file_name);
//
//         switch (campaign)
//         {
//         case (RunIISummer20UL16APV):
//             year_str = "2016preVFP";
//             break;
//         case (RunIISummer20UL16):
//             year_str = "2016postVFP";
//             break;
//         case (RunIISummer20UL17):
//             year_str = "2017";
//             break;
//         case (RunIISummer20UL18):
//             year_str = "2018";
//             break;
//         default:
//             return;
//             break;
//         };
//     };
//
//     double getSF(double pt)
//     {
//         return boohftCalib(year_str, pt);
//     };
//
//     double getSFUp(double pt)
//     {
//         return boohftCalib(year_str, pt, "up");
//     };
//
//     double getSFDn(double pt)
//     {
//         return boohftCalib(year_str, pt, "dn");
//     };
// };

struct VBSVVHXWqqSFs : NanoSFsUL
{
private:
    double boohftCalib(std::string year, double pt, std::string wp, std::string var)
    {
        // https://docs.google.com/presentation/d/1Y39avmA0bSe2hpPdqdM6RZr6s9N_h3iQEVLihOwebJY/edit#slide=id.g28c8a4fdb21_1_0
        float unc_mult = (pt > 800) ? 2.0 : 1.0;

        if (year == "2018")
        {
            /* 2018 Wqq-tagging SF (WP 0.59)
               pT [200, 300) | pT [300, 400) | pT [400, 800)
               0.92 [±0.02] | 0.91 [±0.02] | 0.86 [±0.04]
            */
            if (pt >= 200 && pt < 300)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.92; }
                    else if (var == "up") { return 0.92 + 0.02; }
                    else if (var == "dn") { return 0.92 - 0.02; }
                }
            }
            else if (pt >= 300 && pt < 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.91; }
                    else if (var == "up") { return 0.91 + 0.02; }
                    else if (var == "dn") { return 0.91 - 0.02; }
                }
            }
            else if (pt >= 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.86; }
                    else if (var == "up") { return 0.86 + 0.04 * unc_mult; }
                    else if (var == "dn") { return 0.86 - 0.04 * unc_mult; }
                }
            }
        }
        else if (year == "2017")
        {
            /* 2017 Wqq-tagging SF (WP 0.579)
               pT [200, 300) | pT [300, 400) | pT [400, 800)
               0.98 [+0.04/-0.04] | 0.96 [+0.02/-0.02] | 0.99 [+0.04/-0.05]
            */
            if (pt >= 200 && pt < 300)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.98; }
                    else if (var == "up") { return 0.98 + 0.04; }
                    else if (var == "dn") { return 0.98 - 0.04; }
                }
            }
            else if (pt >= 300 && pt < 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.96; }
                    else if (var == "up") { return 0.96 + 0.02; }
                    else if (var == "dn") { return 0.96 - 0.02; }
                }
            }
            else if (pt >= 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.99; }
                    else if (var == "up") { return 0.99 + 0.05 * unc_mult; }
                    else if (var == "dn") { return 0.99 - 0.04 * unc_mult; }
                }
            }
        }
        else if (year == "2016postVFP")
        {
            /* 2016 Wqq-tagging SF (WP 0.637)
               pT [200, 300) | pT [300, 400) | pT [400, 800)
               0.95 [+0.06/-0.05] | 0.91 [+0.04/-0.04] | 0.89 [+0.07/-0.07]
            */
            if (pt >= 200 && pt < 300)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.95; }
                    else if (var == "up") { return 0.95 + 0.05; }
                    else if (var == "dn") { return 0.95 - 0.06; }
                }
            }
            else if (pt >= 300 && pt < 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.91; }
                    else if (var == "up") { return 0.91 + 0.04; }
                    else if (var == "dn") { return 0.91 - 0.04; }
                }
            }
            else if (pt >= 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.89; }
                    else if (var == "up") { return 0.89 + 0.07 * unc_mult; }
                    else if (var == "dn") { return 0.89 - 0.07 * unc_mult; }
                }
            }
        }
        else if (year == "2016preVFP")
        {
            /* 2016APV Wqq-tagging SF (WP 0.637)
               pT [200, 300) | pT [300, 400) | pT [400, 800)
               0.92 [+0.04/-0.07] | 0.95 [+0.03/-0.04] | 0.94 [+0.06/-0.06]
            */
            if (pt >= 200 && pt < 300)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.92; }
                    else if (var == "up") { return 0.92 + 0.04; }
                    else if (var == "dn") { return 0.92 - 0.07; }
                }
            }
            else if (pt >= 300 && pt < 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.95; }
                    else if (var == "up") { return 0.95 + 0.04; }
                    else if (var == "dn") { return 0.95 - 0.03; }
                }
            }
            else if (pt >= 400)
            {
                if (wp == "trail" || wp == "lead")
                {
                    if (var == "nominal") { return 0.94; }
                    else if (var == "up") { return 0.94 + 0.06 * unc_mult; }
                    else if (var == "dn") { return 0.94 - 0.06 * unc_mult; }
                }
            }
        }
        return 1.;
    };
public:
    std::string year_str;

    VBSVVHXWqqSFs() { /* Do nothing */ };

    void init(TString file_name)
    {
        NanoSFsUL::init(file_name);

        switch (campaign)
        {
        case (RunIISummer20UL16APV):
            year_str = "2016preVFP";
            break;
        case (RunIISummer20UL16):
            year_str = "2016postVFP";
            break;
        case (RunIISummer20UL17):
            year_str = "2017";
            break;
        case (RunIISummer20UL18):
            year_str = "2018";
            break;
        default:
            return;
            break;
        };
    };

    double getSF(double pt, std::string wp)
    {
        return boohftCalib(year_str, pt, wp, "nominal");
    };

    double getSFUp(double pt, std::string wp)
    {
        return boohftCalib(year_str, pt, wp, "up");
    };

    double getSFDn(double pt, std::string wp)
    {
        return boohftCalib(year_str, pt, wp, "dn");
    };
};
// for all merged for jonathan's analysis
// struct VBSVVHXWqqSFs : NanoSFsUL
// {
// private:
//     double boohftCalib(std::string year, double pt, std::string wp, std::string var)
//     {
//         // From these slides: https://docs.google.com/presentation/d/1Y39avmA0bSe2hpPdqdM6RZr6s9N_h3iQEVLihOwebJY/edit?usp=sharing
//         float unc_mult = (pt > 800) ? 2.0 : 1.0;
//
//         if (year == "2018")
//         {
//             if (pt >= 200 && pt < 300)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.91; }
//                     else if (var == "up") { return 0.91+0.02; }
//                     else if (var == "dn") { return 0.91-0.02; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.88; }
//                     else if (var == "up") { return 0.88+0.02; }
//                     else if (var == "dn") { return 0.88-0.02; }
//                 }
//             }
//             else if (pt >= 300 && pt < 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.90; }
//                     else if (var == "up") { return 0.90+0.02; }
//                     else if (var == "dn") { return 0.90-0.02; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.87; }
//                     else if (var == "up") { return 0.87+0.02; }
//                     else if (var == "dn") { return 0.87-0.02; }
//                 }
//             }
//             else if (pt >= 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.85; }
//                     else if (var == "up") { return 0.85+0.04*unc_mult; }
//                     else if (var == "dn") { return 0.85-0.04*unc_mult; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.82; }
//                     else if (var == "up") { return 0.82+0.04*unc_mult; }
//                     else if (var == "dn") { return 0.82-0.04*unc_mult; }
//                 }
//             }
//         }
//         else if (year == "2017")
//         {
//             if (pt >= 200 && pt < 300)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.96; }
//                     else if (var == "up") { return 0.96+0.02; }
//                     else if (var == "dn") { return 0.96-0.02; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.91; }
//                     else if (var == "up") { return 0.91+0.04; }
//                     else if (var == "dn") { return 0.91-0.04; }
//                 }
//             }
//             else if (pt >= 300 && pt < 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.94; }
//                     else if (var == "up") { return 0.94+0.02; }
//                     else if (var == "dn") { return 0.94-0.02; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.91; }
//                     else if (var == "up") { return 0.91+0.02; }
//                     else if (var == "dn") { return 0.91-0.03; }
//                 }
//             }
//             else if (pt >= 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.93; }
//                     else if (var == "up") { return 0.93+0.04*unc_mult; }
//                     else if (var == "dn") { return 0.93-0.04*unc_mult; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.91; }
//                     else if (var == "up") { return 0.91+0.05*unc_mult; }
//                     else if (var == "dn") { return 0.91-0.04*unc_mult; }
//                 }
//             }
//         }
//         else if (year == "2016postVFP")
//         {
//             if (pt >= 200 && pt < 300)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.96; }
//                     else if (var == "up") { return 0.96+0.05; }
//                     else if (var == "dn") { return 0.96-0.05; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.89; }
//                     else if (var == "up") { return 0.89+0.06; }
//                     else if (var == "dn") { return 0.89-0.05; }
//                 }
//             }
//             else if (pt >= 300 && pt < 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.89; }
//                     else if (var == "up") { return 0.89+0.04; }
//                     else if (var == "dn") { return 0.89-0.04; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.87; }
//                     else if (var == "up") { return 0.87+0.04; }
//                     else if (var == "dn") { return 0.87-0.04; }
//                 }
//             }
//             else if (pt >= 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.85; }
//                     else if (var == "up") { return 0.85+0.07*unc_mult; }
//                     else if (var == "dn") { return 0.85-0.06*unc_mult; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.85; }
//                     else if (var == "up") { return 0.85+0.07*unc_mult; }
//                     else if (var == "dn") { return 0.85-0.07*unc_mult; }
//                 }
//             }
//         }
//         else if (year == "2016preVFP")
//         {
//             if (pt >= 200 && pt < 300)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.92; }
//                     else if (var == "up") { return 0.92+0.04; }
//                     else if (var == "dn") { return 0.92-0.04; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.87; }
//                     else if (var == "up") { return 0.87+0.04; }
//                     else if (var == "dn") { return 0.87-0.03; }
//                 }
//             }
//             else if (pt >= 300 && pt < 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.93; }
//                     else if (var == "up") { return 0.93+0.04; }
//                     else if (var == "dn") { return 0.93-0.03; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.86; }
//                     else if (var == "up") { return 0.86+0.04; }
//                     else if (var == "dn") { return 0.86-0.03; }
//                 }
//             }
//             else if (pt >= 400)
//             {
//                 if (wp == "trail")
//                 {
//                     // XWqq > 0.7 (for trailing Vqq fat jet)
//                     if (var == "nominal") { return 0.94; }
//                     else if (var == "up") { return 0.94+0.06*unc_mult; }
//                     else if (var == "dn") { return 0.94-0.06*unc_mult; }
//                 }
//                 else if (wp == "lead")
//                 {
//                     // XWqq > 0.8 (for leading Vqq fat jet)
//                     if (var == "nominal") { return 0.91; }
//                     else if (var == "up") { return 0.91+0.07*unc_mult; }
//                     else if (var == "dn") { return 0.91-0.06*unc_mult; }
//                 }
//             }
//         }
//         return 1.;
//     };
// public:
//     std::string year_str;
//
//     VBSVVHXWqqSFs() { /* Do nothing */ };
//
//     void init(TString file_name)
//     {
//         NanoSFsUL::init(file_name);
//
//         switch (campaign)
//         {
//         case (RunIISummer20UL16APV):
//             year_str = "2016preVFP";
//             break;
//         case (RunIISummer20UL16):
//             year_str = "2016postVFP";
//             break;
//         case (RunIISummer20UL17):
//             year_str = "2017";
//             break;
//         case (RunIISummer20UL18):
//             year_str = "2018";
//             break;
//         default:
//             return;
//             break;
//         };
//     };
//
//     double getSF(double pt, std::string wp)
//     {
//         return boohftCalib(year_str, pt, wp, "nominal");
//     };
//
//     double getSFUp(double pt, std::string wp)
//     {
//         return boohftCalib(year_str, pt, wp, "up");
//     };
//
//     double getSFDn(double pt, std::string wp)
//     {
//         return boohftCalib(year_str, pt, wp, "dn");
//     };
// };

#endif
