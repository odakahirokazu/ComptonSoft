#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import uproot
from tqdm import tqdm
import os
from os.path import join
from scipy.stats import norm
import glob
import ROOT
from scipy.optimize import curve_fit
import h5py

plt.rcParams['font.family']      = "Times New Roman"
plt.rcParams['mathtext.fontset'] = "stix"
plt.rcParams["font.size"]        = 15
plt.rcParams['xtick.labelsize']  = 13
plt.rcParams['ytick.labelsize']  = 13
plt.tight_layout()

C2fC        = 1e15
fC2C        = 1/C2fC
numFEC      = 4
numChPerFEC = 64
LSB         = 0.354167 * fC2C #C
element_charge_fC = 1.602176634e-4
element_charge_C  = element_charge_fC * fC2C
mV2V   = 1e-3
um2cm  = 1e-4
MeV2eV = 1e6
W_eV   = 19.5
alpha0 = 0.21
fC2C     = 1e-15
C2fC     = 1 / fC2C
keV2eV   = 1e3
element_charge_fC = 1.602176634e-4
element_charge_C  = element_charge_fC * fC2C

def gaus(x, norm, mean, std):
    return norm * np.exp(-0.5*(((x-mean)/std)**2))

def norm_gauss(x, mean, std):
    return np.exp(-0.5*((x-mean)/std)**2.0) / ( np.sqrt(2.0 * np.pi) * std)

def func_3d(x, a, b, c, d):
    return a*(x**3) + b*(x**2) + c*x + d

def negative_loglikelihood(x, mean, std):
    return np.sum(np.log(norm_gauss(x, mean, std)))

def cal_hesse(adc0_cmn_fit_array, fit_results):
    x               = np.array(fit_results, dtype=float)
    num_event       = len(adc0_cmn_fit_array)
    grad0_val_array = np.zeros((num_event, 2))

    for i in tqdm(range(len(adc0_cmn_fit_array))):
        grad0 = elementwise_grad(lambda c: \
                    negative_loglikelihood(adc0_cmn_fit_array[i], c[0], c[1]))
        grad0_val_array[i] = grad0(x)

    grad0_val_array = grad0_val_array.T

    A       = np.zeros((2,2))
    A[0][0] = np.mean(grad0_val_array[0]**2)
    A[0][1] = A[1][0] = np.mean(grad0_val_array[0] * grad0_val_array[1])
    A[1][1] = np.mean(grad0_val_array[1]**2)
    hesse   = np.linalg.inv(A) / (num_event)

    return hesse


def perform_TP_fit(filepath_TP, ich, fit_range_width):
    eventtree     = uproot.open(filepath_TP)['eventtree']
    adc_array     = np.vstack( eventtree.arrays(['adc'], 'flag_pseudo==0', library='np')['adc'])
    cmn_array     = np.median(adc_array, axis=1)
    adc_cmn_array = (adc_array - cmn_array[:, np.newaxis]).T
    fit_range     = [np.mean(adc0_cmn_array[ich])-fit_range_width, np.mean(adc_cmn_array[ich])+fit_range_width]


    adc0_cmn_ich_array     = adc0_cmn_array[ich]
    adc0_cmn_ich_fit_array = adc0_cmn_ich_array[(fit_range[0] < adc_cmn_ich_array) & (adc_cmn_ich_array < fit_range[1])]

    results = norm.fit(adc0_cmn_ich_fit_array)

    hesse_matrix = cal_hesse(adc0_cmn_ich_fit_array, results)

    return adc0_cmn_array, results, hesse_matrix


def fitHistogram(filePath, TPCh):
    tfile = ROOT.TFile(filePath, 'r')
    etree = tfile.Get('vatatree')

    adcHistList = []
    funcList    = []
    maxVal      = 0.0
    maxADCList  = []
    resultsList = []
    valueEdgePairList  = []

    for i in range(numFEC):
        adcHistList.append(ROOT.TH1D(f"hist{i}",";Charge (ADC);Count", 1074, -50, 1024))
        cut = f"(index=={TPCh})&&(flag_self_trig!=0)&&(good_event==1)&&(fec_index=={i})&&(hitnum==64)"
        etree.Draw(f"(adc-cmn)>>hist{i}", cut)
        adcHistList[i].SetLineColor(i+1)
        funcList.append(ROOT.TF1(f"func{i}", 'gaus'))
        maxVal = np.max([maxVal, adcHistList[i].GetMaximum()])

        maxADCList.append(adcHistList[i].GetBinCenter(adcHistList[i].GetMaximumBin()))
        funcList[i].SetParameters(adcHistList[i].GetMaximum(), maxADCList[i], 3)
        results = adcHistList[i].Fit(funcList[i], "SEL","", maxADCList[i]-7, maxADCList[i]+7)
        resultsList.append(results)

        # ROOTファイルへ保存
        tmpFileName = "hist.root"
        f = ROOT.TFile(tmpFileName, "RECREATE")
        adcHistList[i].Write()
        f.Close()

        with uproot.open(tmpFileName) as f:
            h = f[f'hist{i}']                     # TH1D を取得
            values, edges = h.to_numpy()    # values: 各binの内容, edges: bin境界(長さ nbins+1)
            valueEdgePairList.append([values, edges])

        os.remove("hist.root")

    return resultsList, valueEdgePairList


def plotGainCurve(meanArray, ccalCoulombArray, coeffArrayADC2fC, coeffArrayccal2ADC, ccalArray, dirFigure):
    numx = 8
    numy = 8
    adcArray = np.linspace(0, 600, 100)
    for iFEC in range(numFEC):
        fig, axes = plt.subplots(numy,numx,figsize=(14,12))
        fig2, axes2 = plt.subplots(numy,numx,figsize=(14,12))

        for ich in tqdm(range(64)):
            iy = ich // numx
            ix = ich % numx
            axes[iy][ix].plot(meanArray.T[ich][iFEC], ccalCoulombArray*C2fC, marker="o", markersize=3, linewidth=0, color="b")

            axes[iy][ix].plot(adcArray, func_3d(adcArray, *coeffArrayADC2fC[iFEC][ich]), color="r")
            axes[iy][ix].set_xlim(0, 600)
            axes[iy][ix].set_ylim(0, 7)

            axes[iy][ix].text(20, 6, f"FEC{iFEC}/{ich}ch", size=9)

            if ix!=0:
                axes[iy][ix].set_yticks([])
            else:
                axes[iy][ix].set_yticks(np.arange(8))
                axes[iy][ix].set_ylabel("Charge (fC)", size=12)

            if iy!=(numy-1):
                axes[iy][ix].set_xticks([])
            else:
                axes[iy][ix].set_xticks(np.arange(0,601,200))
                axes[iy][ix].set_xlabel("ADC Value", size=10)

            axes2[iy][ix].plot(ccalArray, meanArray.T[ich][iFEC], marker="o", markersize=3, linewidth=0, color="b")
            axes2[iy][ix].plot(np.linspace(ccalArray[0], ccalArray[-1],100), func_3d(np.linspace(ccalArray[0], ccalArray[-1],100), *coeffArrayccal2ADC[iFEC][ich]), color="r")
            axes2[iy][ix].set_xlim(0, 18)
            axes2[iy][ix].set_ylim(0, 550)

            axes2[iy][ix].text(2, 500, f"FEC{iFEC}/{ich}ch", size=9)

            if ix!=0:
                axes2[iy][ix].set_yticks([])
            else:
                axes2[iy][ix].set_yticks(np.arange(0,550,100))
                axes2[iy][ix].set_ylabel("ADC Value", size=12)

            if iy!=(numy-1):
                axes2[iy][ix].set_xticks([])
            else:
                axes2[iy][ix].set_xticks(np.arange(0,17,2))
                axes2[iy][ix].set_xlabel("CCal", size=10)

        fig.subplots_adjust(left=0.05, right=0.98, bottom=0.05, top=0.98)
        fig.savefig(f"{dirFigure}/gainCurve_FEC{iFEC}.pdf")

        fig2.subplots_adjust(left=0.05, right=0.98, bottom=0.05, top=0.98)
        fig2.savefig(f"{dirFigure}/gainCurve_ccal_ADC_FEC{iFEC}.pdf")

def makeFitGainCurve(meanArray, ccalCoulombArray, ccalArray):
    coeffArrayADC2fC = np.zeros((numFEC, 64, 4))
    coeffArrayccal2ADC = np.zeros((numFEC, 64, 4))
    for iFEC in range(numFEC):
        for ich in tqdm(range(64)):
            coeffArrayADC2fC[iFEC][ich], _ = curve_fit(func_3d, meanArray.T[ich][iFEC], ccalCoulombArray*C2fC)
            coeffArrayccal2ADC[iFEC][ich], _ = curve_fit(func_3d, ccalArray, meanArray.T[ich][iFEC])

    return coeffArrayADC2fC, coeffArrayccal2ADC


def plotInputChargeVSDigitalCharge(ccalCoulombArray, meanArray, figName="fig/testPulseScanPlot.pdf"):
    numy, numx = (2, 2)
    
    fig, axes = plt.subplots(numy, numx, figsize=(8,6))
    for iFEC in range(numFEC):
        iy = iFEC // numx
        ix = iFEC % numx
    
        for ich in range(64):
            axes[iy][ix].plot(ccalCoulombArray*C2fC, meanArray.T[ich][iFEC], marker="o", markersize=3)
        axes[iy][ix].set_xlim(0, 6)
        axes[iy][ix].set_ylim(0, 600)
        if iy == 1:
            axes[iy][ix].set_xlabel("Charge (fC)")
        if ix == 0:
            axes[iy][ix].set_ylabel("Charge (ADC)")
        axes[iy][ix].set_xticks(np.arange(7))
        axes[iy][ix].set_yticks(np.arange(0, 650, 100))
        axes[iy][ix].text(0.5, 500, f"FEC{iFEC}", size=18)
    fig.savefig(figName)


def plotResponseCurve(dirRoot, dataSubDict, peakArray, meanArray, stdArray, dirFigure):
    numx = 8
    numy = 8
    for iccal, ccal in enumerate(dataSubDict.keys()):
        subDir = dataSubDict[ccal]
        filePaths = glob.glob(join(join(dirRoot, subDir), "*_ch*"))
        fileSortPaths = sorted(filePaths, key=lambda f: int(f.split("_ch")[-1]))
    
        for iFEC in range(numFEC):
            fig, axes = plt.subplots(numy,numx,figsize=(10,8))
    
            for ich in tqdm(range(64)):
                iy = ich // numx
                ix = ich % numx
    
                leafDir = [f for f in fileSortPaths if f.endswith(f"_ch{ich}")][0]
                filePath = join(leafDir, "test_00001_000.root")
                hist = uproot.open(f"{filePath}:FEC_{iFEC};1/hist_cmn_ex_{iFEC}_{ich};1").to_numpy()
                axes[iy][ix].stairs(hist[0], hist[1], color="b")
    
                cenTmp = 0.5 * (hist[1][np.argmax(hist[0])] + hist[1][np.argmax(hist[0])+1])
    
                axes[iy][ix].set_xlim((cenTmp//10)*10-25, (cenTmp//10)*10+25)
                xArray = np.linspace(cenTmp-7, cenTmp+7, 200)
                yArray = gaus(xArray, peakArray[iccal][iFEC][ich], meanArray[iccal][iFEC][ich], stdArray[iccal][iFEC][ich])
                axes[iy][ix].plot(xArray, yArray, color="r")
    
                if iy==(numy-1):
                    axes[iy][ix].set_xlabel("ADC Value")
                if ix==0:
                    axes[iy][ix].set_ylabel("Count")
    
            fig.subplots_adjust(left=0.08, right=0.98, bottom=0.08, top=0.98)
            fig.savefig(f"{dirFigure}/fit_64ch_FEC{iFEC}_ccal{ccal}")


def calcGain(dirRoot, dataSubDict, filename_FEC, outGainFileName, plotFigure):

    #最初の方のテストパルスデータ

    ccalArray        = np.array(list(dataSubDict.keys()), dtype=int)
    ccalCoulombArray = ccalArray * LSB
    
    peakArray    = np.zeros((len(dataSubDict), numFEC, numChPerFEC)) #(ccal, iFEC, ich)
    meanArray    = np.zeros((len(dataSubDict), numFEC, numChPerFEC))
    stdArray     = np.zeros((len(dataSubDict), numFEC, numChPerFEC))
    peakErrArray = np.zeros((len(dataSubDict), numFEC, numChPerFEC))
    meanErrArray = np.zeros((len(dataSubDict), numFEC, numChPerFEC))
    stdErrArray  = np.zeros((len(dataSubDict), numFEC, numChPerFEC))

    for k, (ccal, subDir) in enumerate(dataSubDict.items()):
        filePaths     = glob.glob(f"{dirRoot}/{subDir}/*_ch*")
        fileSortPaths = sorted(filePaths, key=lambda f: int(f.split("_ch")[-1]))
    
        for i in tqdm(range(numChPerFEC)):
            leafDir  = [f for f in fileSortPaths if f.endswith(f"_ch{i}")][0]
            filePath = join(leafDir, filename_FEC)
    
            resultsList, valueEdgePairList = fitHistogram(filePath, i)
            for iFEC in range(4):
                peakArray[k][iFEC][i]    = resultsList[iFEC].Parameter(0)
                meanArray[k][iFEC][i]    = resultsList[iFEC].Parameter(1)
                stdArray[k][iFEC][i]     = resultsList[iFEC].Parameter(2)
                peakErrArray[k][iFEC][i] = resultsList[iFEC].ParError(0)
                meanErrArray[k][iFEC][i] = resultsList[iFEC].ParError(1)
                stdErrArray[k][iFEC][i]  = resultsList[iFEC].ParError(2)

    coeffArrayADC2fC, coeffArrayccal2ADC = makeFitGainCurve(meanArray, ccalCoulombArray, ccalArray)
    
    #save
    with h5py.File(outGainFileName, "w") as f:
        for iFEC in range(numFEC):
            f[f"FEC{iFEC}/ADC2C"]    = coeffArrayADC2fC[iFEC] * fC2C
            f[f"FEC{iFEC}/ccal2ADC"] = coeffArrayccal2ADC[iFEC]


    if plotFigure==True:
        os.makedirs("fig", exist_ok=True)
        plotResponseCurve(  dirRoot, dataSubDict, 
                            peakArray, meanArray, stdArray, 
                            dirFigure="fig")
        plotInputChargeVSDigitalCharge(ccalCoulombArray, meanArray, 
                                        figName="fig/testPulseScanPlot.pdf")
        plotGainCurve(  meanArray, ccalCoulombArray, 
                        coeffArrayADC2fC, coeffArrayccal2ADC, ccalArray, 
                        dirFigure="fig")

#Ekin: keV, F: V/cm
def r_Segreto(Ekin, F, param):
    Wi     = 23.6 #eV
    k     = param[0] * mV2V  #V
    gamma = param[1] / um2cm #/cm

    alpha = 0.227 * (MeV2eV**2)         #eV^2/cm
    beta  = 1.7   *  MeV2eV             #eV/cm
    Ee    = 1 + k * gamma / F

    z  = Ekin * keV2eV * (F + k * beta / Wi) / (k * alpha / Wi)
    R  = ( F / (F + k * beta / Wi) ) * (1 - np.log(1 + z) / z)
    Re = R * Ee

    return Re

def sbpl_inv(Q, Eb, beta1, beta2, Qb, Delta):
    x = Q / Qb
    return Eb * x**beta1 * (1.0 + x**(1.0/Delta))**((beta2-beta1)*Delta)

def convert_keV2Q(E_keV, r):
    return E_keV * keV2eV * r / (W_eV * (1 + alpha0)) * element_charge_C


def calcRecombinationDataBase(E_array, F_array, modelName, outfilename):
    param_set   = {"Scalettar": [3.9, 3.0], "Aprile": [3.7, 2.8], 'Ereditato': [4.2, 2.3]}
    
    numFieldValue = len(F_array)
    Q_array       = np.zeros((F_array.shape[0], E_array.shape[0]))
    
    tgraphList = []
    spl3List   = []
    
    for i, F in enumerate(F_array):
        r = r_Segreto(E_array, F, param_set[modelName])
        Q_array[i] = convert_keV2Q(E_array, r)
        print(f"Q min: {Q_array[i][0]}, Q max: {Q_array[i][-1]}")
        #spline補完
        tgraphList.append(ROOT.TGraph(len(E_array), Q_array[i], E_array))
        spl3List.append(ROOT.TSpline3(f"spl{i}", tgraphList[i]))
        spl3List[i].SetName(f"E{F}")
    
    parList = []
    f = ROOT.TFile(outfilename, "recreate")
    
    for i in range(numFieldValue):
        p0 = (3e2, 0.55, 0.95, 1e-15, 0.3)
        pars, cov = curve_fit(sbpl_inv, Q_array[i], E_array, p0=p0, maxfev=20000)
        spl3List[i].Write()
        parList.append(pars)
    
    f.Close()


if __name__=="__main__":

    productDirectory = "subproducts"
    os.makedirs(productDirectory, exist_ok=True)

    dirRoot = "/Users/takashima/work/grams/run/run9/prerun"
    #dirRoot = "/path/to/run9/prerun/scan_data_directory"
    dataSubDict = {2:  "scan_tp_20250821_231609",
                   4:  "scan_tp_20250802_184214", 
                   6:  "scan_tp_20250821_215446",
                   8:  "scan_tp_20250802_160809", 
                   10: "scan_tp_20250821_204240",
                   12: "scan_tp_20250821_180636",
                   14: "scan_tp_20250821_191921",
                   16: "scan_tp_20250802_172139"
                  }
    plotFigure = False
    filename_FEC    = "test_00001_000.root"
    outGainFileName = "gainFuncParam3d.hdf5"
    outGainFilePath = join(productDirectory, outGainFileName)

    calcGain(dirRoot, dataSubDict, filename_FEC, outGainFilePath, plotFigure)


    #Produce a recombination probability file
    E_array     = np.logspace(0, 4, 99)
    F_array     = np.array([200, 300, 500, 1000, 2000, 3000])
    modelName   = "Aprile"
    outRecombinationFileName = "QvsEkeV_spline.root"
    outRecombinationFilePath = join(productDirectory, outRecombinationFileName)

    #Produce a gain file
    calcRecombinationDataBase(E_array, F_array, modelName, outRecombinationFilePath)

