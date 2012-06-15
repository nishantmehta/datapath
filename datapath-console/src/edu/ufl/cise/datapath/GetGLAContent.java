/**
 * GetGLAContent.java
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

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import edu.ufl.cise.datapath.exception.DataPathError;

/**
 * Servlet implementation class GetGLAContent
 */
@WebServlet ("/GetGLAContent")
public class GetGLAContent extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public GetGLAContent ()
    {
        super ();
    }

    /**
     * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
     *      response)
     */
    @Override
    protected void doGet (HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
        doPost (request, response);
    }

    /**
     * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
     *      response)
     */
    @Override
    protected void doPost (HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
        PrintWriter out = response.getWriter ();
        ConnectDB connDB = new ConnectDB ();
        response.setHeader ("Access-Control-Allow-Origin", "*");
        connDB.connectToDB (DataPathConstants.DATAPATH_DB);
        PreparedStatement preStmt1 = null;
        ResultSet rs1 = null;
        int glaID = 0;
        StringBuffer glaContent = new StringBuffer ();
        try
        {
            glaID = Integer.parseInt (request.getParameter ("gla_id"));
            preStmt1 = connDB._conn.prepareStatement ("SELECT glaLocation FROM GLA_Info WHERE glaID=?");
            preStmt1.setInt (1, glaID);
            rs1 = preStmt1.executeQuery ();
            if (rs1.next ())
            {
                FileInputStream fis = new FileInputStream (rs1.getString ("glaLocation"));
                char current;
                while (fis.available () > 0)
                {
                    current = (char) fis.read ();
                    glaContent.append (current);
                }
            }
            response.setContentType ("text/plain");
            response.setStatus (DataPathConstants.STATUS_OK);
            out.println (glaContent.toString ());
        } catch (SQLException e)
        {
            connDB.rollbackDB ();
            response.setContentType ("application/json");
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.println (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "SQLite Error: "+ e.getMessage ()));
        } catch (FileNotFoundException e)
        {
            response.setContentType ("application/json");
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.println (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "File Error: "+ e.getMessage ()));
        } finally
        {
            connDB.closePRC (preStmt1, rs1);
        }
    }
}
