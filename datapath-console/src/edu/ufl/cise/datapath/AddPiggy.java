/**
 * AddPiggy.java
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

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONException;
import org.json.JSONObject;

import edu.ufl.cise.datapath.exception.DataPathError;

/**
 * Servlet implementation class AddPiggy
 */
@WebServlet ("/AddPiggy")
public class AddPiggy extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public AddPiggy ()
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
        ResultSet rs2 = null;
        PreparedStatement preStmt3 = null;
        String pigName = null, pigDesc = null, pigLoc = null;
        JSONObject result = new JSONObject ();
        try
        {
            pigName = request.getParameter ("pig_name");
            pigDesc = request.getParameter ("pig_desc");
            pigLoc = "/home/psalitra/Work/DataPath/praveen/datapath-console/" + pigName + ".pgy";
            preStmt1 = connDB._conn.prepareStatement ("SELECT * FROM Piggy_Info WHERE pigName=?");
            preStmt1.setString (1, pigName);
            rs1 = preStmt1.executeQuery ();
            if (rs1.next ())
            {
                response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
                out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "A piggy with the name "
                        + pigName
                        + " already exists in the datapath. Please enter a different name."));
            } else
            {
                preStmt2 = connDB._conn.prepareStatement ("INSERT INTO Piggy_Info VALUES (null, ?, ?, ?)");
                preStmt2.setString (1, pigName);
                preStmt2.setString (2, pigDesc);
                preStmt2.setString (3, pigLoc);
                preStmt2.executeUpdate ();
                
                preStmt3 = connDB._conn.prepareStatement ("SELECT MAX(pigID) FROM Piggy_Info WHERE pigName=?");
                preStmt3.setString (1, pigName);
                rs2 = preStmt3.executeQuery ();
                
                if (rs2.next ())
                {
                    result.put ("pig_id", rs2.getInt (1));
                    result.put ("piggy", new JSONObject ().put ("" + rs2.getInt (1), new JSONObject ().put ("pig_name", pigName)
                                                                                                    .put ("pig_desc", pigDesc)));
                }
                PrintStream pigFile = new PrintStream (new FileOutputStream (pigLoc));
                pigFile.flush ();
                pigFile.close ();
            }
            response.setStatus (DataPathConstants.STATUS_OK);
            out.print (result.toString ());
        } catch (SQLException e)
        {
            connDB.rollbackDB ();
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "SQLite Error: "+ e.getMessage ()));
        } catch (FileNotFoundException e)
        {
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "File Error: "+ e.getMessage ()));
        } catch (JSONException e)
        {
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "JSON Error: "+ e.getMessage ()));
        } finally
        {
            connDB.closePR (preStmt1, rs1);
            connDB.closePR (preStmt2, null);
            connDB.closePRC (preStmt3, rs2);
        }
    }
}
