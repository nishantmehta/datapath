/**
 * ConnectDB.java
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
package edu.ufl.cise.datapath;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class ConnectDB
{
    public Connection _conn = null;

    public void connectToDB (String dbURL)
    {
        try
        {
            Class.forName ("org.sqlite.JDBC").newInstance ();
            _conn = DriverManager.getConnection (dbURL);
        } catch (Exception e)
        {
            System.err.println ("Cannot connect to database server. " + e);
        }
    }

    public void commitDB ()
    {
        try
        {
            _conn.commit ();
        } catch (Exception e)
        {
            System.err.println ("Cannot commit to database server. " + e);
        }
    }

    public void rollbackDB ()
    {
        if (_conn != null)
        {
            try
            {
                _conn.rollback ();
            } catch (Exception e)
            {
                System.err.println ("Cannot rollback from database server. "
                        + e);
            }
        }
    }
    
    //Helpers to close DB Resources
    public void closePRC (PreparedStatement preStmt, ResultSet rs) {
        if (preStmt != null)
        {
            try
            {
                preStmt.close ();
            } catch (SQLException e)
            {
                /* exception ignored */
            }
        }
        if (rs != null)
        {
            try
            {
                rs.close ();
            } catch (SQLException e)
            {
                /* exception ignored */
            }
        }
        if (_conn != null)
        {
            try
            {
                _conn.close ();
            } catch (SQLException e)
            {
                /* exception ignored */
            }
        }
    }

    public void closePR (PreparedStatement preStmt, ResultSet rs) {
        if (preStmt != null)
        {
            try
            {
                preStmt.close ();
            } catch (SQLException e)
            {
                /* exception ignored */
            }
        }
        if (rs != null)
        {
            try
            {
                rs.close ();
            } catch (SQLException e)
            {
                /* exception ignored */
            }
        }
    }  
}
