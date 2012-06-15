/**
 * Logger.java
 *
 * @author Praveen Salitra (praveen@cise.ufl.edu)
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
package edu.ufl.cise.datapath.utils;

import java.util.logging.ConsoleHandler;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.SimpleFormatter;

public class Logger
{
    private Logger ()
    {
        _logger = java.util.logging.Logger.getLogger ("edu.ufl.cise.datapath");
        _logger.setUseParentHandlers (false);
        ConsoleHandler cHandler = new ConsoleHandler ();
        _logger.addHandler (cHandler);
        try
        {
            FileHandler fHandler = new FileHandler ("/usr/local/datapath/log/datapath-console.log", MAX_FILE_SIZE, MAX_FILE_COUNT, true);
            fHandler.setFormatter (new SimpleFormatter ());
            _logger.addHandler (fHandler);
        } catch (Exception ex)
        {
            _logger.warning ("File handler could not be initialized. Using console only.");
        }
    }

    public static synchronized Logger getLogger ()
    {
        if (INSTANCE == null)
        {
            INSTANCE = new Logger ();
        }
        return INSTANCE;
    }

    public void info (String message)
    {
        if (_logger.isLoggable (Level.INFO))
        {
            _logger.info (message);
        }
    }

    public void warning (String message)
    {
        if (_logger.isLoggable (Level.WARNING))
        {
            _logger.warning (message);
        }
    }

    public void error (String message)
    {
        if (_logger.isLoggable (Level.SEVERE))
        {
            _logger.severe (message);
        }
    }

    public void error (Throwable t)
    {
        error (t.getMessage ());
    }

    private static Logger            INSTANCE       = null;
    private java.util.logging.Logger _logger;
    private static final int         MAX_FILE_SIZE  = 1000000;
    private static final int         MAX_FILE_COUNT = 1;
}
