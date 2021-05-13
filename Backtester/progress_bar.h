#pragma once

/// <summary>
/// Show a progress bar.
/// </summary>
/// <param name="progress">Fractional value of the progress (interval [0,1]).</param>
void progress_bar(double progress);

/// <summary>
/// Clears the progress bar.
/// </summary>
void progress_bar_clear();

/// <summary>
/// Shows a progress bar.
/// </summary>
/// <param name="val">Value in the interval [0, max]</param>
/// <param name="min">Minimum value.</param>
/// <param name="max">Maximum value.</param>
/// <param name="dt">Value interval.</param>
void progress_bar(double val, double min, double max, double dt);