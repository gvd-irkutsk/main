import pandas as pd
import numpy as np
from scipy.optimize import minimize, Bounds
import matplotlib.pyplot as plt


# Constants
N = 1.37
C = 299792458*1e-9

class ParticleTrack(x0, y0, z0, theta, phi):
    def __init__(self):
        self.x0 = x0
        self.y0 = y0
        self.z0 = z0
        self.theta = theta
        self.phi = phi

    def fit(data):
        ''' Reconstructs the muon track by single string data.
        Takes the data as pandas dataframe contains of  time, x, y, z, amplitude.
        Returns the reconstructed muon track.'''
        
        return self

    def plot(self, geometry):
        # TODO realize geometry loading
        ...
        plt.show()


def t_theor(t0, x0, y0, z0, theta, phi, n=N, c=C, beta=np.deg2rad(41)):
    coors = data_masked[['X', 'Y', 'Z']]
    x1, y1, z1 = coors["X"], coors["Y"], coors["Z"]
    dx, dy, dz = np.cos(phi) * np.sin(theta), np.sin(phi) * np.sin(theta), np.cos(theta)
    d = np.array([dx, dy, dz])
    s = np.array([x1-x0, y1-y0, z1-z0])
    norm_s = np.linalg.norm(s, axis=0)
    norm_d = np.linalg.norm(d)
    α = np.arccos(np.dot(d, s) / (norm_d * norm_s))
    k = norm_s*np.sin(α)
    l2 = k / np.sin(beta)
    l1 = norm_s*np.cos(α) - l2*np.cos(beta)
    t1 = l1 / c
    t2 = l2 / (c / n)
    t_theor = t0 + t1 + t2
    return t_theor

def chi2(params, N_hit=data_masked.shape[0], sigma=3.8, data):
    t0, x0, y0, z0, θ, ϕ = params
    t_theor_values = t_theor(t0, x0, y0, z0, θ, ϕ)
    chi2_value = 1 / (N_hit - 5) * np.sum((data['time'] - t_theor_values) ** 2 / sigma ** 2)
    return chi2_value






