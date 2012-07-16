/**
 * RunPiggy.java
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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
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
 * Servlet implementation class RunPiggy
 */
@WebServlet ("/RunPiggy")
public class RunPiggy extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public RunPiggy ()
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
        response.setContentType ("application/json");
        response.setHeader ("Access-Control-Allow-Origin", "*");
        connDB.connectToDB (DataPathConstants.DATAPATH_DB);
        PreparedStatement preStmt1 = null;
        ResultSet rs1 = null;
        PreparedStatement preStmt2 = null;
        int pigID;
        try
        {
            pigID = Integer.parseInt (request.getParameter ("pig_id"));
            preStmt1 = connDB._conn.prepareStatement ("SELECT pigLocation FROM Piggy_Info WHERE pigID=?");
            preStmt1.setInt (1, pigID);
            rs1 = preStmt1.executeQuery ();
            if (rs1.next ())
            {
	            Runtime.getRuntime().exec(DataPathConstants.DATAPATH_RUN_PIGGY + " " + rs1.getString ("pigLocation"));
            }
            
            response.setStatus (DataPathConstants.STATUS_OK);
        } catch (SQLException e)
        {
            connDB.rollbackDB ();
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "SQLite Error: "+ e.getMessage ()));
        } finally
        {
            connDB.closePR (preStmt1, rs1);
            connDB.closePRC (preStmt2, null);
        }
    }
}
